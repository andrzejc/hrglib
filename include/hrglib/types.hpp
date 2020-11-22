/**
 * @file hrglib/types.hpp
 * @brief Declarations of commonly used HrgLib types.
 *
 * Lightweight header which removes need for including full type definitions,
 * reducing dependency propagation and improving on build time.
 */
#pragma once
#include <cstddef>
#include <string>

//! @brief Root of HrgLib namespace hierarchy; contains all C++ symbols.
namespace hrglib {
using std::size_t;
using std::string;

enum struct relation_name;
class relation;
template<relation_name> class relation_;
template<relation_name> struct relation_traits;
template<class NodeType, typename Comparator> class relation_iterator;

enum struct feature_name;
class features;
template<feature_name> struct feature_traits;

class node;
template<relation_name> class node_;
template<class NodeType> class node_navigator;

class graph;
}  // namespace hrglib
