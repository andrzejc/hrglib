#pragma once
#include "hrglib/types.hpp"
#include "hrglib/type_traits.hpp"
#include "hrglib/relation_traits.hpp"
#include "hrglib/relation_name.hpp"

#include <typeinfo>

namespace hrglib { namespace detail {
[[noreturn]]
void throw_bad_dereference(const std::type_info& t);

template<class NodeType>
class node_navigator_base {
protected:
    NodeType* pos_;
    NodeType* deref() const {
        if (nullptr == pos_) {
            throw_bad_dereference(typeid(NodeType));
        }
        return pos_;
    }

    constexpr explicit node_navigator_base(NodeType* pos) noexcept: pos_{pos} {}

public:
    using node_type = NodeType;

    constexpr explicit operator bool() const noexcept { return pos_ != nullptr; }

    constexpr NodeType* get() const noexcept { return pos_; }

#define HRGLIB_NODE_NAVIGATOR_EVAL(...) \
        do { if (!!*this) { \
            return __VA_ARGS__ ; \
        } else { \
            return {}; \
        } } while (false)

    bool in(relation_name rel) const noexcept {
        HRGLIB_NODE_NAVIGATOR_EVAL(pos_->in(rel));
    }
    bool in(const relation& rel) const noexcept {
        HRGLIB_NODE_NAVIGATOR_EVAL(pos_->in(rel));
    }
    auto as(relation_name rel) const noexcept -> decltype(pos_->as(rel)) {
        HRGLIB_NODE_NAVIGATOR_EVAL(pos_->as(rel));
    }
    template<relation_name rel>
    auto as() const noexcept -> decltype(pos_->template as<rel>()) {
        HRGLIB_NODE_NAVIGATOR_EVAL(pos_->template as<rel>());
    }
    template<class OtherNodeType>
    auto as() const noexcept -> decltype(pos_->template as<OtherNodeType>()) {
        HRGLIB_NODE_NAVIGATOR_EVAL(pos_->template as<OtherNodeType>());
    }

    template<typename Callable>
    constexpr auto apply(Callable&& call) const -> decltype(std::forward<Callable>(call)(*pos_)) {
        HRGLIB_NODE_NAVIGATOR_EVAL(std::forward<Callable>(call)(*pos_));
    }

    node_type* operator->() const { return deref(); }
    node_type& operator*() const { return *deref(); }

    constexpr operator node_type*() const noexcept { return pos_; }

    constexpr auto next() const noexcept -> decltype(pos_->next()) {
        HRGLIB_NODE_NAVIGATOR_EVAL(pos_->next());
    }
    constexpr auto prev() const noexcept -> decltype(pos_->prev()) {
        HRGLIB_NODE_NAVIGATOR_EVAL(pos_->prev());
    }
};

template<class NodeType, typename Enable = void>
struct node_has_parent:
    std::integral_constant<bool, parent_relation<NodeType::RELATION_NAME> != R::INVALID> {};

template<class NodeType>
struct node_has_parent<NodeType, std::enable_if_t<std::is_same<node, std::remove_cv_t<NodeType>>::value>>:
    std::true_type {};


template<class NodeType, bool has_parent = node_has_parent<NodeType>::value>
class node_navigator_with_parent_: public node_navigator_base<NodeType> {
    using base = node_navigator_base<NodeType>;

protected:
    using base::base;

public:
    constexpr auto parent() const noexcept -> decltype(base::pos_->parent()) {
        HRGLIB_NODE_NAVIGATOR_EVAL(base::pos_->parent());
    }
};

template<class NodeType>
struct  node_navigator_with_parent_<NodeType, false>: public node_navigator_base<NodeType> {
    using base = node_navigator_base<NodeType>;

protected:
    using base::base;
};

template<class NodeType, typename Enable = void>
struct node_has_children:
    std::integral_constant<bool, child_relation<NodeType::RELATION_NAME> != R::INVALID> {};

template<class NodeType>
struct node_has_children<NodeType, std::enable_if_t<std::is_same<node, std::remove_cv_t<NodeType>>::value>>:
    std::true_type {};

template<class NodeType, bool has_children = node_has_children<NodeType>::value>
struct node_navigator_with_children_: public node_navigator_with_parent_<NodeType> {
    using base = node_navigator_with_parent_<NodeType>;

protected:
    using base::base;

public:
    constexpr auto first_child() const noexcept -> decltype(base::pos_->first_child()) {
        HRGLIB_NODE_NAVIGATOR_EVAL(base::pos_->first_child());
    }
    constexpr auto last_child() const noexcept -> decltype(base::pos_->last_child()) {
        HRGLIB_NODE_NAVIGATOR_EVAL(base::pos_->last_child());
    }
};

template<class NodeType>
struct node_navigator_with_children_<NodeType, false>: public node_navigator_with_parent_<NodeType> {
    using base = node_navigator_with_parent_<NodeType>;

protected:
    using base::base;
};
}  // namespace detail

template<class NodeType>
class node_navigator: public detail::node_navigator_with_children_<NodeType> {
    using base = detail::node_navigator_with_children_<NodeType>;

public:
    constexpr node_navigator(NodeType* pos = nullptr) noexcept: base{pos} {}
    constexpr explicit node_navigator(NodeType& pos) noexcept: node_navigator{&pos} {}
    template<class NodeTypeOrSubclass>
    constexpr node_navigator(const node_navigator<NodeTypeOrSubclass>& n) noexcept:
        node_navigator{n.get()} {}
};

template<typename T, typename U>
constexpr bool operator==(const node_navigator<T>& lhs, const node_navigator<U>& rhs) noexcept {
    return lhs.get() == rhs.get();
}

template<typename T, typename U>
constexpr bool operator!=(const node_navigator<T>& lhs, const node_navigator<U>& rhs) noexcept {
    return lhs.get() != rhs.get();
}

#if (__cpp_deduction_guides >= 201703L)
template<class NodeType>
node_navigator(NodeType* n) -> node_navigator<NodeType>;
template<class NodeType>
node_navigator(const node_navigator<NodeType>& n) -> node_navigator<NodeType>;
template<class NodeType>
explicit node_navigator(NodeType& n) -> node_navigator<NodeType>;
#endif

namespace detail {
template<class TargetType, class SourceType, typename = enable_if_node_t<SourceType>>
constexpr node_navigator<copy_const_t<SourceType, TargetType>>
        static_node_cast(node_navigator<SourceType> node) noexcept
{
    return {static_copy_const_cast<TargetType>(node.get())};
}
}}  // namespace hrglib::detail
