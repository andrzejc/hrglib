/**
 * @file hrglib/string.hpp
 * @brief Proxy header pulling `string` and `string_view` into HrgLib and adding
 *     some common string utilites.
 */
#pragma once
#include <string>  // IWYU pragma: export

#if defined(__has_include) && !!__has_include(<string_view>)
# include <string_view>  // IWYU pragma: export
namespace hrglib { using std::string_view; }
#else
# include <boost/utility/string_view.hpp>  // IWYU pragma: export
# include <boost/functional/hash.hpp>
# include <functional>

namespace hrglib { using boost::string_view; }
namespace std {
template<>
struct hash<hrglib::string_view> {
    std::size_t operator()(const hrglib::string_view& s) const {
        return boost::hash_range(s.begin(), s.end());
    }
};
}
#endif

namespace hrglib {
using std::string;

template<typename T>
T from_string(string_view s);
}
