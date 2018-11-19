#pragma once
#include "hrglib/types.hpp"

#include <type_traits>

namespace hrglib {

template<typename From, typename To>
using copy_const_t = std::conditional_t<std::is_const_v<From>, std::add_const_t<To>, To>;

template<class TargetType, class SourceType>
constexpr copy_const_t<SourceType, TargetType>* static_copy_const_cast(SourceType* src) noexcept {
    return static_cast<copy_const_t<SourceType, TargetType>*>(src);
}

template<class TargetType, class SourceType>
constexpr copy_const_t<SourceType, TargetType>& static_copy_const_cast(SourceType& src) noexcept {
    return static_cast<copy_const_t<SourceType, TargetType>&>(src);
}

template<class TargetType, class ThisType, class SourceType>
constexpr copy_const_t<ThisType, TargetType>* static_member_cast(ThisType&, copy_const_t<ThisType, SourceType>* mem) noexcept {
    return static_cast<copy_const_t<ThisType, TargetType>*>(mem);
}

template<class TargetType, class ThisType, class SourceType>
constexpr copy_const_t<ThisType, TargetType>& static_member_cast(ThisType&, copy_const_t<ThisType, SourceType>& mem) noexcept {
    return static_cast<copy_const_t<ThisType, TargetType>&>(mem);
}

template<class NodeType>
using enable_if_node_t = std::enable_if_t<std::is_base_of<node, std::remove_cv_t<NodeType>>::value>;

namespace detail {
template<class TargetType, class SourceType, typename = enable_if_node_t<SourceType>>
constexpr copy_const_t<SourceType, TargetType>* static_node_cast(SourceType* node) noexcept {
    return static_copy_const_cast<TargetType>(node);
}

template<class TargetType, class SourceType, typename = enable_if_node_t<SourceType>>
constexpr copy_const_t<SourceType, TargetType>& static_node_cast(SourceType& node) noexcept {
    return static_copy_const_cast<TargetType>(node);
}
}

template<class RelationType>
using enable_if_relation_t = std::enable_if_t<std::is_base_of<relation, std::remove_cv_t<RelationType>>::value>;

namespace detail {
template<class TargetType, class SourceType, typename = enable_if_relation_t<SourceType>>
constexpr copy_const_t<SourceType, TargetType>* static_relation_cast(SourceType* node) noexcept {
    return static_copy_const_cast<TargetType>(node);
}

template<class TargetType, class SourceType, typename = enable_if_relation_t<SourceType>>
constexpr copy_const_t<SourceType, TargetType>& static_relation_cast(SourceType& node) noexcept {
    return static_copy_const_cast<TargetType>(node);
}
}}
