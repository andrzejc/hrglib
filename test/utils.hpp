#pragma once

#include <string>
#include <cstdlib>

namespace hrglib::test {

using std::string;

inline string data_dir() {
    if (auto from_env = std::getenv("HRGLIB_TEST_DATA_DIR")) {
        return from_env;
    } else {
        return ".";
    }
}

template<typename Str>
inline string data_file(Str&& str) {
    return data_dir() + "/" + std::forward<Str>(str);
}

}
