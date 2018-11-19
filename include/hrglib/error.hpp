/**
 * @file hrglib/error.hpp
 * @brief Exception classes used by hrglib.
 */
#pragma once
#include "hrglib/types.hpp"
#include "hrglib/string.hpp"

#include <stdexcept>
#include <typeinfo>
#include <type_traits>
#include <iosfwd>

namespace hrglib {
//! @brief Exception classes and error-handling utilities.
namespace error {

string demangle(const std::type_info& ti);

struct bad_topology: std::runtime_error {
    using std::runtime_error::runtime_error;
    bad_topology(): bad_topology{"bad topology"} {}
};

struct relation_exists: bad_topology {
    const relation_name relation;
    explicit relation_exists(relation_name rel):
        bad_topology{"relation exists"},
        relation{rel}
    {}
};

struct bad_relation: bad_topology {
    const relation_name relation;
    explicit bad_relation(relation_name rel):
        bad_topology{"bad relation"},
        relation{rel}
    {}
};

struct null_navigator_dereference: std::logic_error {
    const std::type_info& type;
    null_navigator_dereference(const std::type_info& type):
        logic_error{"null node dereference"},
        type{type}
    {}
};

struct parsing_error: std::runtime_error {
    using std::runtime_error::runtime_error;
    parsing_error(): parsing_error{"parsing error"} {}
};

struct invalid_feature_name: parsing_error {
    const string feature_name;
    explicit invalid_feature_name(string feature_name):
        parsing_error{"invalid feature name"},
        feature_name{std::move(feature_name)}
    {}
};

struct invalid_feature_type: parsing_error {
    const feature_name feature;
    const std::type_info* const expected_type;
    const std::type_info* const runtime_type;
    explicit invalid_feature_type(
            const string& msg,
            feature_name feature,
            const std::type_info* const expected_type = nullptr,
            const std::type_info* const runtime_type = nullptr
    ):
        parsing_error{msg},
        feature{feature},
        expected_type{expected_type},
        runtime_type{runtime_type}
    {}
    explicit invalid_feature_type(
            feature_name feature,
            const std::type_info* const expected_type = nullptr,
            const std::type_info* const runtime_type = nullptr
    ):
        invalid_feature_type{"invalid feature type", feature, expected_type, runtime_type}
    {}
};

struct invalid_feature_value: parsing_error {
    const feature_name feature;
    const string value;
    explicit invalid_feature_value(
            feature_name feature,
            string value
    ):
        parsing_error{"invalid feature value"},
        feature{feature},
        value{std::move(value)}
    {}
};

struct invalid_relation_name: parsing_error {
    const string relation_name;
    explicit invalid_relation_name(string relation_name):
        parsing_error{"invalid relation name"},
        relation_name{std::move(relation_name)}
    {}
};

}}  // namespace hrglib::error
