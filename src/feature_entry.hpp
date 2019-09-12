#pragma once
#include "hrglib/feature_name.hpp"
#include "hrglib/string.hpp"

#include <typeinfo>

namespace hrglib::detail {
struct feature_entry {
    const feature_name name;
    const string_view string;
    const std::type_info& type;
    const string_view description;

    static const feature_entry& for_(feature_name feat);
    static const feature_entry& for_(string_view feat);
};
}
