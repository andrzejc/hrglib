/**
 * @file hrglib/feature_name.hpp
 * @brief Definition of `hrglib::feature_name` enum and related APIs.
 */
#pragma once
#include "hrglib/feature_list.hpp"
#include "hrglib/string.hpp"

#include <iosfwd>

namespace hrglib {
//! @brief Defines feature labels as compile-time constants; generated
//!     from `HRGLIB_FEATURE_LIST()` macro.
enum struct feature_name {
#define HRGLIB_FEATURE_ENUMERATOR(label, type, comment)  label,
    HRGLIB_FEATURE_LIST(HRGLIB_FEATURE_ENUMERATOR)
#undef HRGLIB_FEATURE_ENUMERATOR
    //! @brief "metadata" constant for performing compile-time validity checks;
    //!     number of defined feature labels.
    COUNT
};

//! @brief Default implementation of `features::name_mapper_type`, get `feature` enumerator
//!     value for textual @p name.
//! @throw error::invalid_feature_name if invalid.
template<>
feature_name from_string<feature_name>(string_view name);

string_view to_string_view(feature_name feat);
inline string to_string(feature_name feat) { return string{to_string_view(feat)}; }

//! @brief Convenience short alias.
using F = feature_name;

//! @brief Describes compile-time properties of feature at label @p feat.
//! @tparam feat feature type to specialize this traits for.
template<feature_name feat>
struct feature_traits {
    //! @brief Type of feature, as it is stored inside `any` instance.
    //!     Defaults to `string` if not explicitly specialized.
    using type = string;
};

//! @brief Type of value stored at @p feat.
//! @tparam feat feature label to get value type for.
template<feature_name feat>
using feature_t = typename feature_traits<feat>::type;

namespace detail {
//! @brief Implements `feature_traits` interface with type overriden to @p Type.
template<typename Type>
struct feature_traits_impl {
    //! @brief Aliases template param @p Type.
    using type = Type;
};
}

//! @brief Emit specialization of `feature_traits` for `feature` label @p feat, using @p type as a
//!     value type.
//! @param feat unqualified (without `F::` of `feature::` part) name of feature type to implement.
//! @param type C++ type used by the feature.
#define HRGLIB_FEATURE_TYPE(feat, type) \
template<> struct feature_traits<F:: feat> : detail::feature_traits_impl<type> {}

// TODO CogME this should be generated from features.yaml as needed
HRGLIB_FEATURE_TYPE(start_pos, size_t);
HRGLIB_FEATURE_TYPE(end_pos,   size_t);
}  // namespace hrglib
