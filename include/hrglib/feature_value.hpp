#pragma once
#include "hrglib/feature_name.hpp"
#include "hrglib/feature_types.hpp"
#include "hrglib/string.hpp"

#include <boost/variant.hpp>

namespace hrglib {
using feature_value = boost::variant<HRGLIB_FEATURE_TYPE_LIST_DIRECT>;

feature_value parse_feature_value(feature_name name, const string& s);
string format_feature_value(feature_name name, const feature_value& fv);
}
