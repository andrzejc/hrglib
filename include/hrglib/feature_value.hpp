#pragma once
#include "hrglib/feature_name.hpp"
#include "hrglib/feature_types.hpp"
#include "hrglib/string.hpp"

#include <variant>

namespace hrglib {
#define HRGLIB_FEATURE_TYPE_VISITOR(name, type, ...) type,
using feature_value = std::variant<
	HRGLIB_FEATURE_LIST(HRGLIB_FEATURE_TYPE_VISITOR)
	std::monostate>;
#undef HRGLIB_FEATURE_TYPE_VISITOR


feature_value parse_feature_value(feature_name name, const string& s);
string format_feature_value(feature_name name, const feature_value& fv);
inline string to_string(const feature_value& fv) {
	return format_feature_value(static_cast<feature_name>(fv.index()), fv);
}
}
