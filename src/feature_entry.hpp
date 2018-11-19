#pragma once
#include "hrglib/feature_name.hpp"
#include "hrglib/string.hpp"

#include <typeinfo>

namespace hrglib { namespace detail {
struct feature_entry {
    const feature_name name;
    const string_view string;
    const std::type_info& type;

    static const feature_entry& for_(feature_name feat);
};
}

template<>
detail::feature_entry from_string<detail::feature_entry>(string_view s);

}
