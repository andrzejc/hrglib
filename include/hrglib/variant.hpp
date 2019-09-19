#pragma once

#if defined(__has_include) && !!__has_include(<variant>)
# include <variant>
namespace hrglib {
using std::variant;
using std::get;
template<typename... Args>
auto variant_index(const variant<Args...>& v) -> decltype(auto) { return v.index(); }
}
// #elif __has_include(<experimental/variant>)
// # include <experimental/variant>
// namespace hrglib {
// using std::experimental::variant;
// using std::experimental::get;
// }
#else
# include <boost/variant.hpp>
namespace hrglib {
using boost::variant;
using boost::get;
template <typename... Args>
auto variant_index(const variant<Args...>& v) -> decltype(auto) { return v.which(); }
}
#endif
