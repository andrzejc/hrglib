/**
 * @file hrglib/format.hpp
 * @brief Pulls boost::format and boost::str into hrglib namespace.
 */
#pragma once
#include <boost/format.hpp>  // IWYU pragma: export

namespace hrglib {
using boost::format;
using boost::str;
}
