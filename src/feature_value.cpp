#include "hrglib/feature_value.hpp"
#include "hrglib/feature_name.hpp"
#include "hrglib/feature_traits.hpp"
#include "hrglib/error.hpp"

#include "utils.hpp"

#include <boost/lexical_cast.hpp>

#include <sstream>
#include <type_traits>

namespace hrglib {
namespace {
const string_view PRE_INTS[] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
};
}

namespace detail {
template<feature_name name>
feature_value feature_value_from_string(feature_name, const string& s) {
	using FeatureType = feature_t<name>;
	try {
		if constexpr (std::is_unsigned_v<FeatureType>) {
			if (!s.empty() && s.front() == '-') {
				throw error::feature_value_error{s, name, typeid(FeatureType)};
			}
		}
		return feature_value{std::in_place_index<static_cast<size_t>(name)>, boost::lexical_cast<FeatureType>(s)};
	} catch (boost::bad_lexical_cast& ex) {
		throw error::feature_value_error{s, name, typeid(FeatureType)};
	}
}

template<feature_name name>
string feature_value_to_string(feature_name, const feature_value& fv) {
	using FeatureType = feature_t<name>;
	auto& val = std::get<static_cast<size_t>(name)>(fv);
	if constexpr (std::is_integral_v<FeatureType>) {
		auto i = static_cast<std::intmax_t>(val);
		if (i >= 0 && i < std::size(PRE_INTS)) {
			return string{PRE_INTS[i]};
		}
	}
	if constexpr (std::is_arithmetic_v<FeatureType>) {
		return std::to_string(val);
	}
	std::ostringstream ss;
	ss << std::get<static_cast<size_t>(name)>(fv);
	return ss.str();
}
}

namespace {
using parser_fn = feature_value(*)(feature_name, const string&);
const parser_fn PARSERS[] = {
#define PARSER(name, type, comment) &detail::feature_value_from_string<F::name>,
	HRGLIB_FEATURE_LIST(PARSER)
};

using formatter_fn = string(*)(feature_name, const feature_value&);
const formatter_fn FORMATTERS[] = {
#define FORMATTER(name, type, comment) &detail::feature_value_to_string<F::name>,
	HRGLIB_FEATURE_LIST(FORMATTER)
};
}

feature_value parse_feature_value(feature_name name, const string& s) {
	return at_enum(PARSERS, name)(name, s);
}

string format_feature_value(feature_name name, const feature_value& fv) {
	return at_enum(FORMATTERS, name)(name, fv);
}
}
