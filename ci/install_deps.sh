#!/usr/bin/env bash
set -eux
set -o pipefail

function cmake_install() {
    local dir="${1}"
    shift
    local build_dir="${dir}/build"
    mkdir -p "${build_dir}"
    ( cd "${build_dir}" && cmake -GNinja "$@" .. )
    cmake --build "${build_dir}"
    # ( cd "${build_dir}" && ctest --output-on-failure )
    sudo $(command -v cmake) --build "${build_dir}" --target install
}

[[ ! -x ci/install_deps.${TRAVIS_OS_NAME}.sh ]] || \
    "ci/install_deps.${TRAVIS_OS_NAME}.sh"

cmake_install deps/gsl-lite \
    -DGSL_LITE_OPT_BUILD_TESTS=OFF
cmake_install deps/yaml-cpp \
    -DYAML_CPP_BUILD_TESTS=OFF \
    -DYAML_CPP_BUILD_TOOLS=OFF
cmake_install deps/googletest

sudo -H python3 -m pip install virtualenv
