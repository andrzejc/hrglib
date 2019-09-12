/**
 * @file hrglib/feature_traits.hpp
 * @brief Definition of `hrglib::feature_traits` and specializations for defined features.
 */
#pragma once
#include "hrglib/feature_name.hpp"
#include "hrglib/feature_list.hpp"
#include "hrglib/feature_types.hpp"

namespace hrglib {
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

#define HRGLIB_FEATURE_TYPE_VISITOR(feat, type, ignore) \
    HRGLIB_FEATURE_TYPE(feat, type);

HRGLIB_FEATURE_LIST(HRGLIB_FEATURE_TYPE_VISITOR)
}
