/**
 * @file hrglib/string.hpp
 * @brief Proxy header pulling `string` and `string_view` into HrgLib and adding
 *     some common string utilites.
 */
#pragma once
#include <string_view>  // IWYU pragma: export
#include <string>  // IWYU pragma: export

namespace hrglib {
using std::string_view;
using std::string;

template<typename T>
T from_string(string_view s);
}
