#include "hrglib/feature_value.hpp"
#include "hrglib/feature_name.hpp"
#include "hrglib/error.hpp"

#include "utils.hpp"

#include <boost/lexical_cast.hpp>

#include <sstream>

namespace hrglib {
namespace detail {
template<typename FeatureType>
feature_value feature_value_from_string(feature_name name, string s) {
	try {
		if constexpr (std::is_unsigned_v<FeatureType>) {
			if (!s.empty() && s.front() == '-') {
				throw error::invalid_feature_type{std::move(s), name, typeid(FeatureType)};
			}
		}
		return feature_value{boost::lexical_cast<FeatureType>(std::move(s))};
	} catch (boost::bad_lexical_cast& ex) {
		throw error::invalid_feature_type{std::move(s), name, typeid(FeatureType)};
	}
}

template<typename FeatureType>
string feature_value_to_string(feature_name, const feature_value& fv) {
	std::ostringstream ss;
	ss << boost::get<FeatureType>(fv);
	return ss.str();
}
}

namespace {
using parser_fn = feature_value(*)(feature_name, string);
const parser_fn PARSERS[] = {
#define PARSER(name, type, comment) &detail::feature_value_from_string<type>,
	HRGLIB_FEATURE_LIST(PARSER)
};

using formatter_fn = string(*)(feature_name, const feature_value&);
const formatter_fn FORMATTERS[] = {
#define FORMATTER(name, type, comment) &detail::feature_value_to_string<type>,
	HRGLIB_FEATURE_LIST(FORMATTER)
};
}

feature_value parse_feature_value(feature_name name, string s) {
	return at_enum(PARSERS, name)(name, std::move(s));
}

string format_feature_value(feature_name name, const feature_value& fv) {
	return at_enum(FORMATTERS, name)(name, fv);
}
}
