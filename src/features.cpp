#include "yaml-cpp_config.hpp"
#include "hrglib/features.hpp"
#include "hrglib/error.hpp"

#include "feature_entry.hpp"
#include "yaml-cpp.hpp"
#include "utils.hpp"

#include <unordered_map>
#include <functional>
#include <typeinfo>
#include <type_traits>
#include <fstream>
#include <iostream>
#include <sstream>

namespace hrglib {
namespace {

//! Parse JSON @p value into @c any of type @p FeatureType.
//! Use struct not funtion because of powerful partial
//! specialization/enable_if implementation matching, which
//! is impossible with ordinary functions.
template<typename FeatureType, typename = void>
struct yaml_parser {
    //! @param feat_label aids error localization.
    static any read(feature_name feat_label, const YAML::Node& value) {
        try {
            return {value.as<FeatureType>()};
        } catch (YAML::BadConversion& ex) {
            string val = value.IsScalar() ? value.Scalar() : "";
            throw error::invalid_feature_type{ex.what(), std::move(val), feat_label, typeid(FeatureType)};
        }
    }
};

template<typename FeatureType>
struct yaml_parser<FeatureType, std::enable_if_t<std::is_unsigned<FeatureType>::value>> {
    //! @param feat_label aids error localization.
    static any read(feature_name feat_label, const YAML::Node& value) {
        try {
            FeatureType val = value.as<FeatureType>();
            if (value.Scalar().front() == '-') {
                throw error::invalid_feature_type{value.Scalar(), feat_label, typeid(FeatureType)};
            }
            return {std::move(val)};
        } catch (YAML::BadConversion& ex) {
            string val = value.IsScalar() ? value.Scalar() : "";
            throw error::invalid_feature_type{ex.what(), std::move(val), feat_label, typeid(FeatureType)};
        }
    }
};

//! Parse JSON @p value into @c any of type @p FeatureType.
//! Use struct not funtion because of powerful partial
//! specialization/enable_if implementation matching, which
//! is impossible with ordinary functions.
template<typename FeatureType, typename = void>
struct formatter {
    static string format(feature_name feat_label, const any& val) {
        std::ostringstream s;
        s << any_cast<const FeatureType&>(val);
        return s.str();
    }
};

using feature_value_from_yaml_t = any (*)(feature_name, const YAML::Node& value);
using feature_to_string_t = string(*)(feature_name, const any& val);

using type_info_ref_t = std::reference_wrapper<const std::type_info>;
struct type_info_hasher {
    std::size_t operator()(const std::type_info& ti) const {
        return ti.hash_code();
    }
};

using parser_map_t = std::unordered_map<
        type_info_ref_t,
        std::pair<feature_value_from_yaml_t, feature_to_string_t>,
        type_info_hasher,
        std::equal_to<std::type_info>>;

using parser_map_entry_t = typename parser_map_t::value_type;

#define PARSER_MAP_ENTRY(feat, type, comment) \
    parser_map_entry_t{ \
        {typeid(feature_t<F:: feat >)}, \
        {&yaml_parser<feature_t<F:: feat >>::read, &formatter<feature_t<F:: feat >>::format} \
    },

const parser_map_t parser_map = {
    HRGLIB_FEATURE_LIST(PARSER_MAP_ENTRY)
};

any feature_value_from_yaml(const YAML::Node& value, feature_name feat) {
    if (auto pe = map_find(parser_map, detail::feature_entry::for_(feat).type)) {
        return (pe->first)(feat, value);
     } else {
        assert(!"parser map missing parser for feature label with valid feature entry");
        throw std::logic_error{"missing yaml parser for feature " + to_string(feat)};
    }
}
}  // namespace

features features::from_yaml(const YAML::Node& object, const name_mapper_type& name_mapper) {
    if (!object.IsMap()) {
        throw error::parsing_error{"features YAML node is not object"};
    }
    auto nm = name_mapper
            ? name_mapper
            : DEFAULT_NAME_MAPPER;
    features res;
    res.reserve(object.size());
    for (auto&& prop: object) {
        const auto feat = nm(prop.first.as<string>());
        res.emplace(feat, feature_value_from_yaml(prop.second, feat));
    }
    return res;
}

features features::from_string(string_view json, const name_mapper_type& name_mapper) {
    return features::from_yaml(YAML::Load(string{json}), name_mapper);
}

features features::from_stream(std::istream& is, const name_mapper_type& name_mapper) {
    return features::from_yaml(YAML::Load(is), name_mapper);
}

features features::from_file(string_view path, const name_mapper_type& name_mapper) {
    std::ifstream fs{string{path}.c_str()};
    return features::from_stream(fs, name_mapper);
}

YAML::Emitter& operator << (YAML::Emitter& out, const features& feats) {
    out << YAML::BeginMap;
    for (auto&& feat: feats) {
        out << YAML::Key << to_string(feat.first);
        if (auto pe = map_find(parser_map, detail::feature_entry::for_(feat.first).type)) {
            out << YAML::Value << (pe->second)(feat.first, feat.second);
        } else {
            assert(!"parser map missing formatter for feature label with valid feature entry");
            throw std::logic_error{"missing formatter for feature " + to_string(feat.first)};
        }
    }
    return out << YAML::EndMap;
}

std::ostream& operator<<(std::ostream& os, const features& feats) {
    YAML::Emitter out{os};
    out << feats;
    return os;
}

std::istream& operator>>(std::istream& is, features& feats) {
    auto f = features::from_stream(is);
    if (!is.fail()) {
        feats = std::move(f);
    }
    return is;
}
}  // namespace hrglib
