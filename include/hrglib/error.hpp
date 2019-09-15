/**
 * @file hrglib/error.hpp
 * @brief Exception classes used by hrglib.
 */
#pragma once
#include "hrglib/types.hpp"
#include "hrglib/string.hpp"
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

//! @brief Exception thrown when operation would result in invalid graph topology.
struct bad_topology: std::runtime_error {
    using std::runtime_error::runtime_error;
    bad_topology(): bad_topology{"bad topology"} {}
};

//! @breif Exception throw when attempting to add node handle in relation which already
struct relation_exists: bad_topology {
    const relation_name relation;
    explicit relation_exists(relation_name rel):
        bad_topology{boost::str(boost::format("node already has relation %1%") % to_string(rel))},
        relation{rel}
    {}
};

struct bad_relation: bad_topology {
    const relation_name relation;
    explicit bad_relation(relation_name rel):
        bad_topology{boost::str(boost::format("relation %1% is not allowed") % to_string(rel))},
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

struct invalid_feature_name: parsing_error {
    const string feature_name;
    explicit invalid_feature_name(string feature_name):
        parsing_error{boost::str(boost::format("feature name %1% is invalid") % feature_name)},
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
        invalid_feature_type{
            boost::str(boost::format("value %1% in not covertible to type %2% when reading feature %3%")
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
