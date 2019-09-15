#pragma once
#include "hrglib/node.hpp"
#include "hrglib/node_navigator.hpp"
#include "hrglib/relation_name.hpp"
#include "hrglib/relation_iterator.hpp"
#include "hrglib/relation_traits.hpp"
#include "hrglib/string.hpp"
#include "hrglib/type_traits.hpp"
#include "hrglib/memory.hpp"

#ifdef HRGLIB_YAMLCPP_PUBLIC
#include <yaml-cpp/emitter.h>
#endif

#include <unordered_set>
#include <functional>
#include <iterator>

namespace hrglib {
class graph;

class relation: private std::unordered_set<unique_ptr<node>> {
    using base = std::unordered_set<unique_ptr<node>>;
    hrglib::graph& graph_;
    const relation_name name_;
    node* first_ = nullptr;
    node* last_ = nullptr;

    template<typename Comparator = node::equal, class Relation>
    static constexpr relation_iterator<copy_const_t<Relation, node>, Comparator> end_(Relation& r) noexcept {
        return nullptr != r.last_
                ? ++decltype(end_(r)){r.last_}
                : r.last_;
    }

#ifdef HRGLIB_YAMLCPP_PUBLIC
    friend YAML::Emitter& operator << (YAML::Emitter& out, const hrglib::graph& g);
#endif

protected:
    explicit relation(hrglib::graph& g, relation_name rel):
        graph_{g},
        name_{rel}
    {}

    relation(relation&&) = default;
    relation& operator=(relation&&) = default;

    relation& set_first_(node* n) {
        first_ = n;
        return *this;
    }
    relation& set_last_(node* n) {
        last_ = n;
        return *this;
    }

public:
    virtual ~relation() = default;

    using factory_type = std::function<unique_ptr<relation>(hrglib::graph&, relation_name)>;
    struct default_factory {
        unique_ptr<relation> operator()(hrglib::graph& g, relation_name rel) const;
    };

    using name_mapper_type = std::function<relation_name(string_view)>;
    static constexpr auto DEFAULT_NAME_MAPPER = from_string<relation_name>;

    using node_type = node;
    using const_navigator = node_navigator<const node>;
    using navigator = node_navigator<node>;
    using const_iterator = relation_iterator<const node>;
    using iterator = relation_iterator<node>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    constexpr const hrglib::graph& graph() const noexcept { return graph_; }
    constexpr hrglib::graph& graph() noexcept { return graph_; }

    constexpr relation_name name() const noexcept { return name_; }
    //! Relation is open when it is like SylSegment: only first node makes sense but not last.
    constexpr bool is_open() const noexcept { return last_ != nullptr; }

    constexpr const_navigator first() const noexcept { return {first_}; }
    constexpr navigator first() noexcept { return {first_}; }
    relation& set_first(node *n);

    constexpr const_navigator last() const noexcept { return {last_}; }
    constexpr navigator last() noexcept { return {last_}; }
    relation& set_last(node *n);

    constexpr const_iterator begin() const noexcept { return {first_}; }
    constexpr const_iterator cbegin() const noexcept { return begin(); }
    constexpr iterator begin() noexcept { return {first_}; }

    constexpr const_iterator end() const noexcept { return end_(*this); }
    constexpr const_iterator cend() const noexcept { return end(); }
    constexpr iterator end() noexcept { return end_(*this); }

    void erase(node& n);
    using base::size;
    //! Create a loose node in this relation.
    node& create(node* in_other_relation = nullptr);
    //! Create a node, appending it to the end of relation as new end.
    node& append(node* in_other_relation = nullptr);

    node& prepend(node* in_other_relation = nullptr);
};

template<relation_name rel>
class relation_: public relation {
    using base = relation;
    using node_t = hrglib::node_t<rel>;
    // using parent_node_t = typename relation_traits<rel>::parent_type;
    // using child_node_t = typename relation_traits<rel>::children_type;

    template<class Relation>
    static constexpr auto first_(Relation& r) noexcept {
        return detail::static_node_cast<node_t>(r.base::first());
    }

    template<class Relation>
    static constexpr auto last_(Relation& r) noexcept {
        return detail::static_node_cast<node_t>(r.base::last());
    }

    template<class Relation>
    static constexpr auto begin_(Relation& r) noexcept {
        return detail::static_node_cast<node_t>(r.base::begin());
    }

    template<class Relation>
    static constexpr auto end_(Relation& r) noexcept {
        return detail::static_node_cast<node_t>(r.base::end());
    }

    friend relation::default_factory;

protected:
    explicit relation_(hrglib::graph& g):
        base{g, rel}
    {}

public:
    static constexpr auto NAME = rel;

    using node_type = node_t;
    using const_navigator = node_navigator<const node_t>;
    using navigator = node_navigator<node_t>;

    using const_iterator = relation_iterator<const node_t>;
    using iterator = relation_iterator<node_t>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    constexpr const_navigator first() const noexcept { return first_(*this); }
    constexpr navigator first() noexcept { return first_(*this); }
    relation_& set_first(node_t* n) { base::set_first_(n); return *this; }
    relation& set_first(node*) = delete;

    constexpr const_navigator last() const noexcept { return last_(*this); }
    constexpr navigator last() noexcept { return last_(*this); }
    relation_& set_last(node_t *n) { base::set_last_(n); return *this; }
    relation& set_last(node*) = delete;

    constexpr const_iterator begin() const noexcept { return begin_(*this); }
    constexpr const_iterator cbegin() const noexcept { return begin(); }
    constexpr iterator begin() noexcept { return begin_(*this); }

    constexpr const_iterator end() const noexcept { return end_(*this); }
    constexpr const_iterator cend() const noexcept { return end(); }
    constexpr iterator end() noexcept { return end_(*this); }

    node_t& create(node* in_other_relation = nullptr) {
        return static_cast<node_t&>(base::create(in_other_relation));
    }

    node_t& append(node* in_other_relation = nullptr) {
        return static_cast<node_t&>(base::append(in_other_relation));
    }

    node_t& prepend(node* in_other_relation = nullptr) {
        return static_cast<node_t&>(base::prepend(in_other_relation));
    }
    void erase(node_t& n) { base::erase(n); }
    void erase(node&) = delete;
};
}  // namespace hrglib
