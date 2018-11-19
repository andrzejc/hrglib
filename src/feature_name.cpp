#include "hrglib/feature_name.hpp"
#include "hrglib/error.hpp"
#include "hrglib/types.hpp"
#include "hrglib/not_null.hpp"

#include "feature_entry.hpp"
#include "utils.hpp"

#include <unordered_map>
#include <cassert>
#include <iterator>
#include <utility>
#include <cstddef>

namespace hrglib { namespace {
using detail::feature_entry;

// TODO CogME this should be auto-generated from features.yaml
const feature_entry feat_entries[] = {
#define FEATURE_ENTRY(feat, type, comment) { F:: feat, #feat, typeid(feature_t<F:: feat >) },
    HRGLIB_FEATURE_LIST(FEATURE_ENTRY)
};

static_assert(std::size(feat_entries) == static_cast<std::size_t>(feature_name::COUNT),
        "feat_entries array size mismatch");

const std::unordered_map<string_view, not_null<const feature_entry*>> name_to_feat = {
#define NAME_TO_FEAT(feat, type, comment) { #feat, &feat_entries[static_cast<std::size_t>(F:: feat)] },
    HRGLIB_FEATURE_LIST(NAME_TO_FEAT)
};
}  // namespace
namespace detail {
const feature_entry& feature_entry::for_(feature_name feat) {
    return at_enum(feat_entries, feat);
}
}  // namespace detail

template<>
detail::feature_entry from_string<detail::feature_entry>(string_view s) {
    if (auto it = name_to_feat.find(s); it != name_to_feat.end()) {
        assert(s == it->second->string);
        return *it->second;
    } else {
        throw error::invalid_feature_name{string{s}};
    }
}

template<>
feature_name from_string<feature_name>(string_view s) {
    return from_string<detail::feature_entry>(s).name;
}

string_view to_string_view(feature_name feat) {
    return feature_entry::for_(feat).string;
}
}  // namespace hrglib
