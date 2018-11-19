/**
 * @file hrglib/memory.hpp
 * @brief Proxy header pulling `unique_ptr` into HrgLib and some other memory-related stuff.
 */
#pragma once
#include <memory>  // IWYU pragma: export

namespace hrglib {
using std::unique_ptr;
}
