/**
 * @file hrglib/relation_iterator.hpp
 * @brief Defines `relation_iterator` template class.
 */
#pragma once
#include "hrglib/node_navigator.hpp"
#include "hrglib/type_traits.hpp"
#include "hrglib/iterator.hpp"

#include <iterator>
#include <functional>

namespace hrglib {
//! @brief Traverses HRG across horizontal relations (using `node::next()` and `node::prev()` operations).
//! @tparam NodeType (possibly const) node or relation-specific subclass.
//! @tparam Comparator functor comparing the @p NodeType instances (and consequently relation iterators)
//!     for equality. Can be either `std::equal_to<node>` AKA `node::equal`, `node::contents_equal` or
//!     anything else fitting the interface. With `node::contents_equal` the iterator will consider
//!     all node relation handles sharing the same contents as the same.
template<class NodeType, typename Comparator = std::equal_to<node>>
class relation_iterator:
        public std::iterator<std::bidirectional_iterator_tag, NodeType>,
        public iterator::with_sentinel<relation_iterator<NodeType, Comparator>>,
        public iterator::with_reverse_sentinel<relation_iterator<NodeType, Comparator>>,
        private Comparator
{
    using base = Comparator;
    using comp_t = Comparator;
    using nav_t = node_navigator<NodeType>;
    // Having both current and prev allows to implement bidirectional iterator, otherwise
    // backtracking from `relation::end()` would not be possible.
    nav_t curr_, prev_;

public:
    //! @brief Construct from `node_navigator` specialized for compatible node type.
    template<class NodeTypeOrSubclass>
    constexpr relation_iterator(node_navigator<NodeTypeOrSubclass> pos, comp_t comp = comp_t{}):
        base{std::move(comp)}, curr_{pos}, prev_{curr_.prev()} {}

    constexpr relation_iterator(NodeType *pos, comp_t comp = comp_t{}):
        relation_iterator{node_navigator<NodeType>{pos}, std::move(comp)} {}

    constexpr explicit relation_iterator(NodeType& pos, comp_t comp = comp_t{}):
        relation_iterator{&pos, std::move(comp)} {}

    constexpr const nav_t& nav() const noexcept { return curr_; }

    constexpr const comp_t& comparator() const { return *this; }

    constexpr explicit operator bool() const noexcept { return bool{curr_}; }

    //! @throw null_navigator_dereference if at end.
    NodeType& operator*() const { return *nav(); }

    relation_iterator& operator++() {
        if (!curr_) {
            throw std::out_of_range{"increment past end"};
        }
        curr_ = (prev_ = curr_).next();
        return *this;
    }
    relation_iterator operator++(int) {
        auto res = *this;
        ++(*this);
        return res;
    }
    relation_iterator& operator--() {
        if (!prev_) {
            throw std::out_of_range{"decrement before begin"};
        }
        prev_ = (curr_ = prev_).prev();
        return *this;
    }
    relation_iterator operator--(int) {
        auto res = *this;
        --(*this);
        return res;
    }
};

template<class T, class U, class Comparator>
constexpr bool operator==(const relation_iterator<T, Comparator>& lhs, const relation_iterator<U, Comparator>& rhs) noexcept {
    return (!lhs.nav() && !rhs.nav())
            || (lhs.nav() && rhs.nav()
                && lhs.comparator()(*lhs.nav(), *rhs.nav()));
}
template<class T, class U, class Comparator>
constexpr bool operator!=(const relation_iterator<T, Comparator>& lhs, const relation_iterator<U, Comparator>& rhs) noexcept {
    return !(lhs == rhs);
}

namespace detail {
template<class TargetType, class SourceType, typename Comparator, typename = enable_if_node_t<SourceType>>
constexpr relation_iterator<copy_const_t<SourceType, TargetType>, Comparator>
        static_node_cast(relation_iterator<SourceType, Comparator> n) noexcept
{
    return {static_node_cast<TargetType>(n.nav())};
}
}

template<class NodeType, typename Comparator = std::equal_to<node>>
using relation_ahead = iterator::range_with_sentinel<relation_iterator<NodeType, Comparator>>;
template<class NodeType, typename Comparator = std::equal_to<node>>
using relation_before = iterator::reverse_range_with_sentinel<relation_iterator<NodeType, Comparator>>;
}  // namespace hrglib
