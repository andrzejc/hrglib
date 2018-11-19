/**
 * @file hrglib/any.hpp
 * @brief Proxy header pulling std or boost implementation of `any` and `any_cast` into hrglib.
 */
#pragma once

#if __has_include(<any>)
# include <any>
namespace hrglib {
using std::any;
using std::any_cast;
}
// #elif __has_include(<experimental/any>)
// # include <experimental/any>
// namespace hrglib {
// using std::experimental::any;
// using std::experimental::any_cast;
// }
#else
# include <boost/any.hpp>
namespace hrglib {
using boost::any;
using boost::any_cast;
}
#endif
