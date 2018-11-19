/**
 * @file hrglib/node.hpp
 * @brief Definition of `hrglib::node` and its family of `hrglib::node_<relation_name>` subclasses.
 */
#pragma once
#include "hrglib/node_navigator.hpp"
#include "hrglib/relation_name.hpp"
#include "hrglib/relation_traits.hpp"
#include "hrglib/relation_iterator.hpp"
#include "hrglib/memory.hpp"
#include "hrglib/type_traits.hpp"

#include <functional>
#include <unordered_map>

namespace hrglib {
class features;
class graph;
class relation;

//! @brief Represents a HRG graph node in some specific `relation` which is known only
//!    at runtime.
class node {
    //! @brief Represents shared node contents; contains the `features` and `node_map` instances.
    struct contents;
    //! @brief Holds the contents shared among `node` handles in different relations.
    contents& cont_;
    //! @brief Backreference to `relation` this node belongs to.
    relation& rel_;
    //! @brief Prev node in the same relation.
    node* prev_ = nullptr;
    //! @brief Next node in the same relation.
    node* next_ = nullptr;
    //! @brief Parent node, possibly in other relation.
    node* parent_ = nullptr;
    //! @brief First child of this node, possibly in other relation.
    node* first_child_ = nullptr;
    //! @brief Last child of this node, possibly in other relation, but same
    //! as @c first_child_ (possibly `nullptr` if relation is open).
    node* last_child_ = nullptr;

    //! @brief Attach a handle in given `relation` @p r to the `contents` of node in some other
    //!     relation or create a new contents instance if @p in_other_relation is `nullptr`.
    //! @param r `relation` to which the new `node` will belong.
    //! @param in_other_relation (possibly `nullptr`) pointer to node whose contents will be
    //!     shared with the new `node`.
    //! @return reference to updated or new instance of `contents`.
    //! @throw error::relation_exists if @p in_other_relation's contents already has a `node` in
    //!     which belongs to `relation` @p r.
    contents& attach_contents(const relation& r, node* in_other_relation);
    void release_contents(const relation& r) noexcept;

    //! @brief Remove parent's references to this, via `first_child_` and `last_child_`.
    void unlink_parent();
    //! @brief Remove next's reference to this via `prev_`.
    void unlink_next();
    //! @brief Remove prev's reference to this via `next_`.
    void unlink_prev();
    //! @brief Remove children's references to this via `parent_`.
    void unlink_children();

    // template<class NodeType>
    // static constexpr NodeType& first_(NodeType& self) noexcept {
    //     auto pos = &self;
    //     for (; pos->prev_ != nullptr; pos = pos->prev_) ;
    //     return *pos;
    // }

    template<class NodeType>
    static constexpr NodeType& last_(NodeType& self) noexcept {
        auto pos = &self;
        for (; pos->next_ != nullptr; pos = pos->next_) ;
        return *pos;
    }

    friend class relation;
    using rel_t = relation;
    using rel_name_t = relation_name;

protected:
    // Less evil helper to avoid duplication when writing trivial getters.
    // Should not be used outside this particular use case, hence uglied name.
    template<class ThisType>
    static constexpr ThisType& mutable_(const ThisType& self) noexcept {
        return const_cast<ThisType&>(self);
    }
    //
    explicit node(relation& rel, node *in_other_relation = nullptr):
        cont_{attach_contents(rel, in_other_relation)},
        rel_{rel}
    {}

    node& set_next_(node* n);
    node& set_prev_(node* n);
    node& set_parent_(node* n);
    node& set_first_child_(node* n);
    node& set_last_child_(node* n);
    const std::function<bool(const relation& parent, const relation& child)>& relation_validator() const noexcept;

public:
    //! @brief Type of function used for validating parent<->child links created with `set_parent()` and
    //!     `set_first_child()`/`set_last_child()` APIs. This function is a property of the
    //!     `graph` this `node` belongs to.
    using relation_validator_type = std::function<bool(const relation& parent, const relation& child)>;
    //! @brief Default implementation of `relation_validator_type` which validates the parent<->child relations
    //!     based on the built-in `relation_traits` specializations.
    struct default_relation_validator {
        bool operator()(const relation& parent, const relation& child) const;
    };

    virtual ~node() noexcept;

    //! @return reference to the `graph` this node belongs to.
    const hrglib::graph& graph() const noexcept { return mutable_(*this).graph(); }
    //! @copydoc graph() const
    hrglib::graph& graph() noexcept;

    using node_map = std::unordered_map<hrglib::relation_name, std::reference_wrapper<node>>;
    const node_map& relations() const noexcept;

    const hrglib::features& features() const noexcept { return mutable_(*this).features(); }
    hrglib::features& features() noexcept;

    hrglib::relation_name relation_name() const noexcept;
    constexpr const hrglib::relation& relation() const noexcept { return rel_; }
    constexpr hrglib::relation& relation() noexcept { return rel_; }

    bool in(const hrglib::relation& r) const noexcept;
    bool in(hrglib::relation_name rel) const noexcept;

    using const_navigator = node_navigator<const node>;
    using navigator = node_navigator<node>;

    //! Variants with relation type specified at runtime.
    const_navigator as(hrglib::relation_name rel) const noexcept { return mutable_(*this).as(rel); }
    navigator as(hrglib::relation_name rel) noexcept;

    //! Variants with relation type specified at compile time.
    template<hrglib::relation_name rel>
    node_navigator<const node_t<rel>> as() const noexcept {
        return detail::static_node_cast<node_t<rel>>(as(rel));
    }
    template<hrglib::relation_name rel>
    node_navigator<node_t<rel>> as() noexcept {
        return detail::static_node_cast<node_t<rel>>(as(rel));
    }
    template<class NodeType>
    node_navigator<const NodeType> as() const noexcept {
        return detail::static_node_cast<NodeType>(as(NodeType::RELATION_NAME));
    }
    template<class NodeType>
    node_navigator<NodeType> as() noexcept {
        return detail::static_node_cast<NodeType>(as(NodeType::RELATION_NAME));
    }

    using factory_type = std::function<unique_ptr<node>(hrglib::relation&, node*)>;
    struct default_factory {
        unique_ptr<node> operator()(hrglib::relation&, node *in_other_relation) const;
    };

    constexpr const_navigator nav() const noexcept { return {this}; }
    constexpr navigator nav() noexcept { return {this}; }

    constexpr const_navigator next() const noexcept { return {next_}; }
    constexpr navigator next() noexcept { return {next_}; }
    node& set_next(node* n);

    constexpr const_navigator prev() const noexcept { return {prev_}; }
    constexpr navigator prev() noexcept { return {prev_}; }
    node& set_prev(node* n);

    constexpr const_navigator parent() const noexcept { return {parent_}; }
    constexpr navigator parent() noexcept { return {parent_}; }
    //! @brief Set parent of this node (with validation).
    //! If previous parent has this node as its first or last child, it is unlinked.
    //! @see relation_validator_type
    //! @throw std::invalid_argument if @p p belongs to different `graphs`.
    //! @throw error::bad_relation if @p p has inadequate relation for parent.
    //! @return `*this`
    node& set_parent(node* p);

    constexpr const_navigator first_child() const noexcept { return {first_child_}; }
    constexpr navigator first_child() noexcept { return {first_child_}; }
    node& set_first_child(node* c);

    constexpr const_navigator last_child() const noexcept { return {last_child_}; }
    constexpr navigator last_child() noexcept { return {last_child_}; }
    node& set_last_child(node* c);

    node& insert_next(node* in_other_relation = nullptr);
    node& insert_prev(node* in_other_relation = nullptr);

    // Relation-specific node handles compare as entities.
    friend constexpr bool operator==(const node& lhs, const node& rhs) noexcept {
        return &lhs == &rhs;
    }
    friend constexpr bool operator!=(const node& lhs, const node& rhs) noexcept {
        return !(lhs == rhs);
    }
    friend constexpr bool operator<(const node& lhs, const node& rhs) noexcept {
        return &lhs < &rhs;
    }

    friend struct contents_equal;
    struct contents_equal {
        bool operator()(const node& lhs, const node& rhs) const {
            return &lhs.cont_ == &rhs.cont_;
        }
    };
    using equal = std::equal_to<node>;

    template<typename Comparator = node::equal>
    constexpr relation_ahead<const node, Comparator> ahead() const noexcept { return {nav()}; }
    template<typename Comparator = node::equal>
    constexpr relation_ahead<node, Comparator> ahead() noexcept { return {nav()}; }

    template<typename Comparator = node::equal>
    constexpr relation_before<const node, Comparator> back() const noexcept { return {nav()}; }
    template<typename Comparator = node::equal>
    constexpr relation_before<node, Comparator> back() noexcept { return {nav()}; }
};

namespace detail {
//! @brief Mixin adding support for typed parent access and navigation to plain `node`.
template<class NodeType, relation_name parent_rel>
class node_with_parent_: public node {
protected:
    using node::node;

public:
    static constexpr hrglib::relation_name PARENT_RELATION_NAME = parent_rel;
    using parent_type = node_t<parent_rel>;
    using const_parent_navigator = node_navigator<const parent_type>;
    using parent_navigator = node_navigator<parent_type>;

    constexpr const_parent_navigator parent() const noexcept {
        return detail::static_node_cast<parent_type>(node::parent());
    }
    constexpr parent_navigator parent() noexcept {
        return detail::static_node_cast<parent_type>(node::parent());
    }
    NodeType& set_parent(parent_type* parent) {
        return static_cast<NodeType&>(node::set_parent(parent));
    }
    node& set_parent(node*) = delete;
};

//! @brief Mixin specialization for `R::INVALID` parent relation which disables
//!     the navigation in this direction.
template<class NodeType>
struct node_with_parent_<NodeType, R::INVALID>: public node {
protected:
    using node::node;

public:
    // using parent_type = void;
    const_navigator parent() const = delete;
    navigator parent() = delete;
    node& set_parent(node*) = delete;
};

//! @brief Mixin adding support for typed child access and navigation.
template<class NodeType, relation_name child_rel, relation_name parent_rel>
struct node_with_children_: public node_with_parent_<NodeType, parent_rel> {
    using base = node_with_parent_<NodeType, parent_rel>;

protected:
    using base::base;

public:
    static constexpr hrglib::relation_name CHILD_RELATION_NAME = child_rel;
    using child_type = node_t<child_rel>;
    using const_child_navigator = node_navigator<const child_type>;
    using child_navigator = node_navigator<child_type>;

    constexpr const_child_navigator first_child() const noexcept {
        return detail::static_node_cast<child_type>(node::first_child());
    }
    constexpr child_navigator first_child() noexcept {
        return detail::static_node_cast<child_type>(node::first_child());
    }
    constexpr const_child_navigator last_child() const noexcept {
        return detail::static_node_cast<child_type>(node::last_child());
    }
    constexpr child_navigator last_child() noexcept {
        return detail::static_node_cast<child_type>(node::last_child());
    }
    NodeType& set_first_child(child_type* child) {
        return static_cast<NodeType&>(node::set_first_child(child));
    }
    NodeType& set_last_child(child_type* child) {
        return static_cast<NodeType&>(node::set_last_child(child));
    }
    node& set_first_child(node*) = delete;
    node& set_last_child(node*) = delete;
};

//! @brief Mixin specialization for `R::INVALID` child relation which disables
//!     the navigation in this direction.
template<class NodeType, relation_name parent_rel>
struct node_with_children_<NodeType, R::INVALID, parent_rel>: public node_with_parent_<NodeType, parent_rel> {
    using base = node_with_parent_<NodeType, parent_rel>;

protected:
    using base::base;

public:
    // using child_type = void;
    node::const_navigator first_child() const = delete;
    node::navigator first_child() = delete;
    node::const_navigator last_child() const = delete;
    node::navigator last_child() = delete;
    node& set_first_child(node*) = delete;
    node& set_last_child(node*) = delete;
};
}  // namespace detail

//! @brief Represents the node in specific, compile-time constant, relation identified by @p rel.
//!     Overrides some APIs with the ones featuring narrowed, relation-specific types given by
//!     the schema defined by `relation_traits`.
//! @tparam rel relation label to specialize node for.
template<relation_name rel>
class node_: public detail::node_with_children_<node_t<rel>, child_relation<rel>, parent_relation<rel>> {
    using base = detail::node_with_children_<node_t<rel>, child_relation<rel>, parent_relation<rel>>;
    using rel_t = relation_t<rel>;
    using node_t = node_t<rel>;
    friend node::default_factory;

protected:
    using base::base;

public:
    using relation_type = rel_t;

    constexpr const relation_type& relation() const noexcept {
        return static_cast<const rel_t&>(node::relation());
    }
    constexpr relation_type& relation() noexcept {
        return static_cast<rel_t&>(node::relation());
    }

    static constexpr auto RELATION_NAME = rel;

    using const_navigator = node_navigator<const node_t>;
    using navigator = node_navigator<node_t>;

    constexpr const_navigator nav() const noexcept {
        return detail::static_node_cast<node_t>(node::nav());
    }
    constexpr navigator nav() noexcept {
        return detail::static_node_cast<node_t>(node::nav());
    }
    constexpr const_navigator next() const noexcept {
        return detail::static_node_cast<node_t>(node::next());
    }
    constexpr navigator next() noexcept {
        return detail::static_node_cast<node_t>(node::next());
    }
    constexpr const_navigator prev() const noexcept {
        return detail::static_node_cast<node_t>(node::prev());
    }
    constexpr navigator prev() noexcept {
        return detail::static_node_cast<node_t>(node::prev());
    }
    node_t& set_next(node_t* n) {
        return static_cast<node_t&>(node::set_next(n));
    }
    node_t& set_prev(node_t* n) {
        return static_cast<node_t&>(node::set_prev(n));
    }
    node& set_next(node*) = delete;
    node& set_prev(node*) = delete;

    node_t& insert_next(node* in_other_relation = nullptr) {
        return static_cast<node_t&>(node::insert_next(in_other_relation));
    }
    node_t& insert_prev(node* in_other_relation = nullptr) {
        return static_cast<node_t&>(node::insert_prev(in_other_relation));
    }
};
}  // namespace hrglib
