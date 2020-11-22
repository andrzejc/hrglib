#include "hrglib/feature_name.hpp"
#include "hrglib/feature_traits.hpp"
#include "hrglib/error.hpp"
#include "hrglib/types.hpp"
#include "hrglib/map_find.hpp"

#include "feature_entry.hpp"
#include "utils.hpp"

#include <unordered_map>
#include <cassert>
#include <iterator>
#include <cstddef>

namespace hrglib { namespace {
using detail::feature_entry;

const feature_entry feat_entries[] = {
#define FEATURE_ENTRY(feat, type, comment) { F:: feat, #feat, typeid(feature_t<F:: feat >), comment },
    HRGLIB_FEATURE_LIST(FEATURE_ENTRY)
};

static_assert(std::size(feat_entries) == static_cast<std::size_t>(feature_name::COUNT),
        "feat_entries array size mismatch");

const std::unordered_map<string_view, const feature_entry*> name_to_feat = {
#define NAME_TO_FEAT(feat, type, comment) { #feat, &feat_entries[static_cast<std::size_t>(F:: feat)] },
    HRGLIB_FEATURE_LIST(NAME_TO_FEAT)
};
}  // namespace
namespace detail {
const feature_entry& feature_entry::for_(feature_name feat) {
    return at_enum(feat_entries, feat);
}

const feature_entry& feature_entry::for_(string_view s) {
    if (auto fe = map_find(name_to_feat, s)) {
        assert(s == (*fe)->string);
        return **fe;
    } else {
        throw error::invalid_feature_name{hrglib::string{s}};
    }
}
}  // namespace detail

template<>
feature_name from_string<feature_name>(string_view s) {
    return feature_entry::for_(s).name;
}

string_view to_string_view(feature_name feat) {
    return feature_entry::for_(feat).string;
}
}  // namespace hrglib
