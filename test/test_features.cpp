#include "hrglib/features.hpp"
#include "hrglib/feature_name.hpp"
#include "hrglib/error.hpp"  // IWYU pragma: keep
#include "hrglib/optional.hpp"  // IWYU pragma: keep
#include "hrglib/string.hpp"
#include "hrglib/types.hpp"

#include "utils.hpp"

#include <gtest/gtest.h>

#include <type_traits>
#include <sstream>
#include <stdexcept>

namespace hrglib::test {

TEST(features, at) {
    features feats;

    EXPECT_FALSE(feats.has(F::name));
    auto&& f1 = feats.at<F::name>();
    static_assert(std::is_same_v<string&, decltype(f1)>, "");
    EXPECT_TRUE(feats.has(F::name));
    f1 = "foo";
    EXPECT_EQ(feats.at<F::name>(), "foo");

    EXPECT_FALSE(feats.has(F::start_pos));
    auto&& f2 = feats.at<F::start_pos>();
    static_assert(std::is_same_v<size_t&, decltype(f2)>, "");
    EXPECT_TRUE(feats.has(F::start_pos));
    f2 = 123;
    EXPECT_EQ(feats.at<F::start_pos>(), 123);
}

TEST(features, get) {
    features feats;

    EXPECT_FALSE(feats.has(F::name));
    auto o1 = feats.get<F::name>();
    EXPECT_FALSE(o1);
    EXPECT_FALSE(feats.get(F::name));

    feats.at<F::name>() = "foo";

    EXPECT_TRUE(feats.has(F::name));
    o1 = feats.get<F::name>();
    EXPECT_TRUE(o1);
    EXPECT_EQ(*o1, "foo");
    EXPECT_TRUE(feats.get(F::name));

    EXPECT_FALSE(feats.has(F::start_pos));
    auto o2 = feats.get<F::start_pos>();
    EXPECT_FALSE(o2);
    EXPECT_FALSE(feats.get(F::start_pos));

    feats.at<F::start_pos>() = 123;

    EXPECT_TRUE(feats.has(F::start_pos));
    o2 = feats.get<F::start_pos>();
    EXPECT_TRUE(o2);
    EXPECT_EQ(*o2, 123);
    EXPECT_TRUE(feats.get(F::start_pos));
}

TEST(features, set) {
    features feats;

    EXPECT_FALSE(feats.has(F::name));

    feats.set<F::name>("foo");
    EXPECT_TRUE(feats.has(F::name));
    EXPECT_EQ(feats.at<F::name>(), "foo");
    EXPECT_NE(feats.get(F::name), nullopt);

    EXPECT_FALSE(feats.has(F::start_pos));
    feats.set<F::start_pos>(123);
    EXPECT_TRUE(feats.has(F::start_pos));
    EXPECT_EQ(feats.at<F::start_pos>(), 123);
}

TEST(features, from_string_success) {
    auto json =
R"EOF({
    "name": "foo",
    "punc": "),",
    "prepunc": "(",
    "start_pos": 0,
    "end_pos": 6
})EOF";
    auto feats = features::from_string(json);
    EXPECT_EQ(feats.size(), 5);
    EXPECT_EQ(feats.at<F::name>(), "foo");
    EXPECT_EQ(feats.at<F::punc>(), "),");
    EXPECT_EQ(feats.at<F::prepunc>(), "(");
    EXPECT_FALSE(feats.has(F::whitespace));
    EXPECT_EQ(feats.at<F::start_pos>(), 0);
    EXPECT_EQ(feats.at<F::end_pos>(), 6);
}

TEST(features, from_string_throws) {
    EXPECT_THROW(features::from_string(""), error::parsing_error);
    EXPECT_THROW(features::from_string("[]"), error::parsing_error);
    EXPECT_THROW(features::from_string("1"), error::parsing_error);
    EXPECT_THROW(features::from_string("{ \"foo\": 0 }"), error::feature_name_error);
    EXPECT_THROW(features::from_string("{ \"start_pos\": \"foo\" }"), error::feature_value_error);
    EXPECT_THROW(features::from_string("{ \"start_pos\": 0.1 }"), error::feature_value_error);
    EXPECT_THROW(features::from_string("{ \"start_pos\": -1 }"), error::feature_value_error);
}

TEST(features, from_file_success) {
    EXPECT_NO_THROW(features::from_file(data_file("test_features.json")));
}

TEST(features, from_file_throws) {
    EXPECT_THROW(features::from_file(data_file("ffoo.json")), std::runtime_error);
}

}  // namespace hrglib::test
