#pragma once

#include <cstdio>
#include <system_error>
#include <memory>
#include <cassert>
#include <cerrno>
#include <stdexcept>

namespace hrglib {
// inline auto file_open(const char* path, const char* mode) {
//    std::unique_ptr<std::FILE, decltype(&std::fclose)> fp{
//             std::fopen(path, mode), &std::fclose};
//     if (!fp) {
//         assert(0 != errno);
//         throw std::system_error{errno, std::generic_category()};
//     }
//     return fp;
// }

template<typename Elem, std::size_t N, typename EnumeratorWithCount>
std::enable_if_t<std::is_same_v<EnumeratorWithCount, decltype(EnumeratorWithCount::COUNT)>, Elem&>
     at_enum(Elem (&array)[N], EnumeratorWithCount index)
{
    static_assert(static_cast<std::size_t>(EnumeratorWithCount::COUNT) == N,
        "enumerator count and array size must match");
    const auto si = static_cast<std::size_t>(index);
    if (si >= N) {
        assert(!"index outside range [0, enumerator::COUNT)");
        throw std::out_of_range{"index outside array size"};
    }
    return array[si];
}

}

