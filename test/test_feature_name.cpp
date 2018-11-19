#include "hrglib/feature_name.hpp"
#include "hrglib/feature_list.hpp"
#include "hrglib/error.hpp"  // IWYU pragma: keep

#include <gtest/gtest.h>

namespace hrglib::test {

TEST(feature_name, from_string) {

#define TEST_FEATURE_BY_NAME(name, type, comment) \
    EXPECT_EQ(from_string<feature_name>(#name), F:: name );

    HRGLIB_FEATURE_LIST(TEST_FEATURE_BY_NAME)

    EXPECT_THROW(from_string<feature_name>("foo"), error::invalid_feature_name);
    EXPECT_THROW(from_string<feature_name>(""), error::invalid_feature_name);
    EXPECT_THROW(from_string<feature_name>({}), error::invalid_feature_name);
}

// these tests will abort() on failed assert in debug builds
#if NDEBUG
TEST(feature_name, to_string_throws_on_invalid_feature) {
    EXPECT_THROW(to_string(static_cast<feature_name>(-1)), std::out_of_range);
}
#endif

}  // namespace hrglib::test
