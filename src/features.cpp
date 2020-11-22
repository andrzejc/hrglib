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
features features::from_yaml(const YAML::Node& object) {
    if (!object.IsMap()) {
        throw error::parsing_error{"features YAML node is not object"};
    }
    features res;
    res.reserve(object.size());
    for (auto&& prop: object) {
        const auto feat = hrglib::from_string<feature_name>(prop.first.as<string>());
        try {
            res.emplace(feat, parse_feature_value(feat, prop.second.as<string>()));
        } catch (YAML::BadConversion& ex) {
            string val = prop.second.IsScalar() ? prop.second.Scalar() : "<NotScalar>";
            throw error::invalid_feature_type{ex.what(), std::move(val), feat, detail::feature_entry::for_(feat).type};
        }
    }
    return res;
}

features features::from_string(string_view json) {
    return features::from_yaml(YAML::Load(string{json}));
}

features features::from_stream(std::istream& is) {
    return features::from_yaml(YAML::Load(is));
}

features features::from_file(string_view path) {
    std::ifstream fs{string{path}.c_str()};
    return features::from_stream(fs);
}

YAML::Emitter& operator << (YAML::Emitter& out, const features& feats) {
    out << YAML::BeginMap;
    for (auto&& feat: feats) {
        out << YAML::Key << to_string(feat.first);
        out << YAML::Value << format_feature_value(feat.first, feat.second);
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
