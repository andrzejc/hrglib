/**
 * @file hrglib/error.hpp
 * @brief Exception classes used by hrglib.
 */
#pragma once
#include "hrglib/types.hpp"
#include "hrglib/feature_name.hpp"
#include "hrglib/relation_name.hpp"

#include <boost/format.hpp>

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
        bad_node{boost::str(boost::format("node already has relation %1%") % to_string(rel))},
        relation{rel}
    {}
};

struct bad_relation: bad_node {
    const relation_name relation;
    explicit bad_relation(relation_name rel):
        bad_node{boost::str(boost::format("node in relation %1% is not allowed") % to_string(rel))},
        relation{rel}
    {}
};

struct bad_dereference: std::logic_error {
    const std::type_info& type;
    bad_dereference(const std::type_info& type):
        logic_error{boost::str(boost::format("dereference of null navigator of type %1%") % demangle(type))},
        type{type}
    {}
};

struct parsing_error: std::runtime_error {
    using std::runtime_error::runtime_error;
    parsing_error(): parsing_error{"parsing error"} {}
};

struct feature_name_error: parsing_error {
    const string feature_name;
    explicit feature_name_error(string feature_name):
        parsing_error{boost::str(boost::format("feature name \"%1%\" is invalid") % feature_name)},
        feature_name{std::move(feature_name)}
    {}
};

struct feature_value_error: parsing_error {
    const string value;
    const feature_name feature;
    const std::type_info& expected_type;
    explicit feature_value_error(
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
    explicit feature_value_error(
            string value,
            feature_name feature,
            const std::type_info& expected_type
    ):
        feature_value_error{boost::str(boost::format("value \"%1%\" in not allowed for feature %2% of type %3%")
                % value % to_string_view(feature) % demangle(expected_type)),
            std::move(value), feature, expected_type
        }
    {}
};

struct relation_name_error: parsing_error {
    const string relation_name;
    explicit relation_name_error(string relation_name):
        parsing_error{boost::str(boost::format("relation name \"%1%\" is invalid") % relation_name)},
        relation_name{std::move(relation_name)}
    {}
};

}}  // namespace hrglib::error
