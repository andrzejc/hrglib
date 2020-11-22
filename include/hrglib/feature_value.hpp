#pragma once
#include "hrglib/feature_list.hpp"
#include "hrglib/feature_types.hpp"
#include "hrglib/string.hpp"

#include <boost/variant.hpp>

namespace hrglib {
using feature_value = boost::variant<HRGLIB_FEATURE_TYPE_LIST_DIRECT>;

template<>
feature_value from_string<feature_value>(string_view s);
string to_string(const feature_value& fv);
}
