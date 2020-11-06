/**
 * @file hrglib/not_null.hpp
 * @brief Proxy header pulling `not_null` into HrgLib.
 */
#pragma once
#include <gsl/gsl>  // IWYU pragma: export

namespace hrglib {
using gsl::not_null;
}
