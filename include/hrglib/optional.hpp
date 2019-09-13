/**
 * @file hrglib/optional.hpp
 * @brief Proxy header pulling `optional` and `nullopt` into HrgLib.
 */
#pragma once
// XXX boost::optional supports references out of the box
#include <boost/optional.hpp>  // IWYU pragma: export

namespace hrglib {
using boost::optional;
const auto nullopt = boost::none;
}
