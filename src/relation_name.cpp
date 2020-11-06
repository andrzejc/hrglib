#include "hrglib/relation_name.hpp"
#include "hrglib/relation_list.hpp"
#include "hrglib/string.hpp"
#include "hrglib/error.hpp"
#include "hrglib/types.hpp"
#include "hrglib/map_find.hpp"

#include "utils.hpp"

#include <cstddef>
#include <unordered_map>
#include <iterator>
#include <utility>
#include <iosfwd>

namespace hrglib {
namespace {
const string_view rel_names[] = {
#define RELATION_ENTRY(rel, ...) { #rel },
    HRGLIB_RELATION_LIST(RELATION_ENTRY)
};

static_assert(std::size(rel_names) == static_cast<std::size_t>(R::COUNT),
        "rel_names array size mismatch");

const std::unordered_map<string_view, const relation_name> name_to_rel = {
#define NAME_TO_REL(rel, ...)  { #rel, R:: rel },
    HRGLIB_RELATION_LIST(NAME_TO_REL)
};
}  // namespace

string_view to_string_view(relation_name rel) {
    return at_enum(rel_names, rel);
}

template<>
relation_name from_string<relation_name>(string_view name) {
    if (auto rel = map_find(name_to_rel, name)) {
        return *rel;
    } else {
        throw error::invalid_relation_name{string{name}};
    }
}
}  // namespace hrglib
