#include "hrglib/error.hpp"

#include <cxxabi.h>

#include <memory>
#include <cstdlib>   // for free

namespace hrglib::error {

// namespace {
// #if defined(__clang__)
// constexpr char compiler[] = "Clang/LLVM";
// #elif defined(__ICC) || defined(__INTEL_COMPILER)
// constexpr char compiler[] = "Intel ICC/ICPC";
// #elif defined(__GNUC__) || defined(__GNUG__)
// constexpr char personality[] = "GNU GCC/G++";
// #elif defined(_MSC_VER)
// constexpr char compiler[] = "Microsoft Visual Studio";
// #else
// constexpr char compiler[] = "Unknown";
// #endif
// }

string demangle(const std::type_info& type) {
    int status = 0;
    auto name = std::unique_ptr<char, void(*)(void*)>{
            abi::__cxa_demangle(type.name(), 0, 0, &status), std::free};
    if (0 == status) {
        return {name.get()};
    } else {
        // XXX log some error or sth
        return {type.name()};
    }
}

}
