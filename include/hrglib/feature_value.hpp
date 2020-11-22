#pragma once
#include "hrglib/feature_list.hpp"
#include "hrglib/feature_types.hpp"

#include <variant>

namespace hrglib {
using feature_value = std::variant<HRGLIB_FEATURE_TYPE_LIST>;
}
