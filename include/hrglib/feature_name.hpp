/**
 * @file hrglib/feature_name.hpp
 * @brief Definition of `hrglib::feature_name` enum and related APIs.
 */
#pragma once
#include "hrglib/feature_list.hpp"
#include "hrglib/string.hpp"

namespace hrglib {
//! @brief Defines feature labels as compile-time constants; generated
//!     from `HRGLIB_FEATURE_LIST()` macro.
enum struct feature_name {
#define HRGLIB_FEATURE_ENUMERATOR(label, ...)  label,
    HRGLIB_FEATURE_LIST(HRGLIB_FEATURE_ENUMERATOR)
#undef HRGLIB_FEATURE_ENUMERATOR
    //! @brief "metadata" constant for performing compile-time validity checks;
    //!     number of defined feature labels.
    COUNT
};

//! @brief Default implementation of `features::name_mapper_type`, get `feature` enumerator
//!     value for textual @p name.
//! @throw error::feature_name_error if invalid.
template<>
feature_name from_string<feature_name>(string_view name);

string_view to_string_view(feature_name feat);
inline string to_string(feature_name feat) { return string{to_string_view(feat)}; }

//! @brief Convenience short alias.
using F = feature_name;
}  // namespace hrglib
