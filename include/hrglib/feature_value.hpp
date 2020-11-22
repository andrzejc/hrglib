#pragma once
#include "hrglib/feature_list.hpp"
#include "hrglib/feature_types.hpp"

#include <variant>

namespace hrglib {
namespace detail { struct ignored_ {}; }
using feature_value = std::variant<std::monostate,
#define HRGLIB_FEATURE_TYPE_ENUMERATOR(index, type)  type,
    HRGLIB_FEATURE_TYPE_LIST(HRGLIB_FEATURE_TYPE_ENUMERATOR)
#undef HRGLIB_FEATURE_TYPE_ENUMERATOR
        detail::ignored_>;
}
