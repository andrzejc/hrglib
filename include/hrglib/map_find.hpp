/**
 * @file hrglib/map_find.hpp
 * @brief Definition of `hrglib::map_find()` template function.
 */
#pragma once
#include "hrglib/optional.hpp"
#include "hrglib/type_traits.hpp"

#include <utility>

namespace hrglib {
/**
 * @brief Optional access to the mapped value at @p key.
 *
 * The access is by-reference, ie. the result of this function is an l-value and may
 * be used for assignment (if @p Map type is not `const`).
 *
 * @tparam Map std-interface conforming map type, possibly const (inferred).
 * @tparam Key whatever Map::find() accepts (inferred).
 * @param map to search in.
 * @param key to search for.
 * @return optional<std::add_lvalue_reference_t<copy_const_t<Map, typename Map::mapped_type>>>
 */
template<class Map, typename Key>
optional<std::add_lvalue_reference_t<copy_const_t<Map, typename Map::mapped_type>>> map_find(Map& map, Key&& key) {
    if (auto it = map.find(std::forward<Key>(key)); it != map.end()) {
        return {it->second};
    } else {
        return {};
    }
}
}  // namespace hrglib
