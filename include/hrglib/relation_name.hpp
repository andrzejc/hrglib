/**
 * @file hrglib/relation_name.hpp
 * @brief Definition of `hrglib::relation_name` enum and related APIs.
 */
#pragma once
#include "hrglib/relation_list.hpp"
#include "hrglib/string.hpp"

#include <iosfwd>

namespace hrglib {
//! @brief Defines relation labels as compile-time constants; generated
//!     from `HRGLIB_RELATION_LIST()` macro.
enum struct relation_name {
#define HRGLIB_RELATION_ENUMERATOR(relation, ...)  relation,
    HRGLIB_RELATION_LIST(HRGLIB_RELATION_ENUMERATOR)
#undef HRGLIB_RELATION_ENUMERATOR
    //! @brief "metadata" constant for performing compile-time validity checks;
    //!     number of defined relation labels.
    COUNT,
    //! @brief Nonexistent relation.
    INVALID = -1
};

//! @throw error::relation_name_error if invalid.
template<>
relation_name from_string(string_view name);

string_view to_string_view(relation_name rel);
inline string to_string(relation_name rel) { return string{to_string_view(rel)}; }

//! Convenience alias.
using R = relation_name;
}  // namespace hrglib
