#!/usr/bin/env bash
set -eux
set -o pipefail

function cmake_install() {
    local dir="$1"
    shift
    local build_dir="build/${dir}"
    mkdir -p "${build_dir}"
    build_dir=$(cd "${build_dir}" && pwd)
    local root="$PWD"
    dir=$(cd "${dir}" && pwd)
    (
        cd "${build_dir}"
        cmake \
            "-G${CMAKE_GENERATOR:-Ninja}" \
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-RelWithDebInfo}" \
            "-DCMAKE_INSTALL_PREFIX=${root}/build/deps/install" \
            "$@" \
            "${dir}"
    )
    cmake --build "${build_dir}"
    # ( cd "${build_dir}" && ctest --output-on-failure )
    cmake --build "${build_dir}" --target install
}

[[ ! -x ci/install_deps.${TRAVIS_OS_NAME}.sh ]] || \
    source "ci/install_deps.${TRAVIS_OS_NAME}.sh"

cmake_install deps/gsl-lite \
    -DGSL_LITE_OPT_BUILD_TESTS=OFF
cmake_install deps/yaml-cpp \
    -DYAML_CPP_BUILD_TESTS=OFF \
    -DYAML_CPP_BUILD_TOOLS=OFF
cmake_install deps/googletest \
    -Dgtest_force_shared_crt=ON
# cmake_install deps/boost \
#     -DBOOST_DOWNLOAD_TO_BINARY_DIR=ON

sudo -H python3 -m pip install virtualenv

set +u
set +x
