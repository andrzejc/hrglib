/**
 * @file hrglib/not_null.hpp
 * @brief Proxy header pulling `not_null` into HrgLib.
 */
#pragma once
#if __has_include(<gsl/gsl-lite.hpp>)
#include <gsl/gsl-lite.hpp>  // IWYU pragma: export
#else
#include <gsl/gsl>  // IWYU pragma: export
#endif

namespace hrglib {
using gsl::not_null;
}
