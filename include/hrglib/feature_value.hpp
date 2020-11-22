#pragma once
#include "hrglib/feature_list.hpp"
#include "hrglib/feature_types.hpp"

#if __has_include(<variant>)
# include <variant>

namespace hrglib { namespace detail { namespace variantimpl {
using std::variant;
using std::get;
}}}
#else
# include <boost/variant.hpp>

namespace hrglib { namespace detail { namespace variantimpl {
using boost::variant;
using boost::get;
}}}
#endif

namespace hrglib {
using feature_value = detail::variantimpl::variant<HRGLIB_FEATURE_TYPE_LIST>;
}
