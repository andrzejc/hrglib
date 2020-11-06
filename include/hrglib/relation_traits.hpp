/**
 * @file hrglib/relation_traits.hpp
 * @brief Definition of `hrglib::relation_traits` template and related APIs.
 */
#pragma once
#include "hrglib/types.hpp"
#include "hrglib/relation_list.hpp"
#include "hrglib/relation_name.hpp"
#include "hrglib/relation_types.hpp"

namespace hrglib {
/**
 * @brief Defines compile-time properties of `relation` of type @p Rel.
 *
 * Fully-defined `relation_traits` has the following members:
 * * `typedef node_type`, subclass of `node_<Rel>`;
 * * `typedef relation_type`, subclass of `relation_<Rel>`;
 * * `static constexpr relation_name parent_relation`;
 * * `static constexpr relation_name child_relation`;
 * @tparam Rel `relation_name` whose properties are defined.
 */
template<relation_name Rel>
struct relation_traits;

namespace detail {
//! @brief Implementation of `relation_traits<Rel>`.
template<relation_name Rel,
        class NodeType = node_<Rel>,
        class RelationType = relation_<Rel>,
        relation_name ParentRel = R::INVALID,
        relation_name ChildRel = R::INVALID>
struct relation_traits_impl {
    static constexpr relation_name NAME = Rel;
    using node_type = NodeType;
    using relation_type = RelationType;
    static constexpr relation_name parent_relation = ParentRel;
    static constexpr relation_name child_relation = ChildRel;
};
}

//! @brief Define explicit specialization of `relation_traits` for relation @p rel.
//! @param rel `relation_name` to define.
//! @param node_class value for `node_type` member.
//! @param relation_class value for `relation_type` member.
//! @param parent_rel value for `parent_relation` member.
//! @param child_rel value for `child_relation` member.
#define HRGLIB_RELATION_DEF(rel, node_class, relation_class, parent_rel, child_rel) \
template<> \
struct relation_traits<rel>: \
    detail::relation_traits_impl<rel, node_class, relation_class, parent_rel, child_rel> {}

//! @brief Explicit, undefined specialization of `relation_traits` for nonexistent
//!     relation `relation_name::INVALID`.
template<>
struct relation_traits<R::INVALID>;

//! @brief Convenience compile-time expression for name of parent relation of @p rel.
template<relation_name rel>
constexpr relation_name parent_relation = relation_traits<rel>::parent_relation;

//! @brief Convenience compile-time expression for name of child relation of @p rel.
template<relation_name rel>
constexpr relation_name child_relation = relation_traits<rel>::child_relation;

//! @brief Relation-specific `node` subclass type (eg. `token`, `word`).
template<relation_name rel>
using node_t = typename relation_traits<rel>::node_type;

//! @brief Relation-specific `relation` subclass type.
template<relation_name rel>
using relation_t = typename relation_traits<rel>::relation_type;

#define HRGLIB_RELATION_DEF_VISITOR(rel, node_class, relation_class, parent_rel, child_rel, ...) \
    HRGLIB_RELATION_DEF(R:: rel, node_class, relation_class, R:: parent_rel, R:: child_rel);

HRGLIB_RELATION_LIST(HRGLIB_RELATION_DEF_VISITOR)
}  // namespace hrglib
