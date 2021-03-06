/**
 * @file hrglib/error.hpp
 * @brief Exception classes used by hrglib.
 */
#pragma once
#include "hrglib/types.hpp"
#include "hrglib/string.hpp"
#include "hrglib/feature_name.hpp"
#include "hrglib/relation_name.hpp"
#include "hrglib/format.hpp"

#include <stdexcept>
#include <typeinfo>
#include <type_traits>
#include <iosfwd>

namespace hrglib {
//! @brief Exception classes and error-handling utilities.
namespace error {

string demangle(const std::type_info& ti);

//! @brief Exception thrown when node is inadequate for the operation.
struct bad_node: std::invalid_argument {
    const hrglib::node* const node = nullptr;
    using std::invalid_argument::invalid_argument;
    bad_node(): bad_node{"bad node"} {}
};

//! @brief Exception throw when attempting to add node handle in relation which already exists.
struct relation_exists: bad_node {
    const relation_name relation;
    explicit relation_exists(relation_name rel):
        bad_node{str(format("node already has relation %1%") % to_string(rel))},
        relation{rel}
    {}
};

struct bad_relation: bad_node {
    const relation_name relation;
    explicit bad_relation(relation_name rel):
        bad_node{str(format("node in relation %1% is not allowed") % to_string(rel))},
        relation{rel}
    {}
};

struct bad_dereference: std::logic_error {
    const std::type_info& type;
    bad_dereference(const std::type_info& type):
        logic_error{str(format("dereference of null navigator of type %1%") % demangle(type))},
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
        parsing_error{str(format("feature name \"%1%\" is invalid") % feature_name)},
        feature_name{std::move(feature_name)}
    {}
};

struct invalid_feature_type: parsing_error {
    const string value;
    const feature_name feature;
    const std::type_info& expected_type;
    explicit invalid_feature_type(
            const string& msg,
            string value,
            feature_name feature,
            const std::type_info& expected_type
    ):
        parsing_error{msg},
        value{std::move(value)},
        feature{feature},
        expected_type{expected_type}
    {}
    explicit invalid_feature_type(
            string value,
            feature_name feature,
            const std::type_info& expected_type
    ):
        invalid_feature_type{str(format("value \"%1%\" in not covertible to type %2% when reading feature %3%")
                % value % demangle(expected_type) % to_string(feature)),
            std::move(value), feature, expected_type
        }
    {}
};

struct invalid_feature_value: parsing_error {
    const feature_name feature;
    const string value;
    explicit invalid_feature_value(
            feature_name feature,
            string value
    ):
        parsing_error{str(format("value \"%1%\" is not allowed for feature %2%")
            % value % to_string(feature))},
        feature{feature},
        value{std::move(value)}
    {}
};

struct invalid_relation_name: parsing_error {
    const string relation_name;
    explicit invalid_relation_name(string relation_name):
        parsing_error{str(format("relation name \"%1%\" is invalid") % relation_name)},
        relation_name{std::move(relation_name)}
    {}
};

}}  // namespace hrglib::error
