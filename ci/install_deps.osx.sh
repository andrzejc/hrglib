#!/usr/bin/env bash
set -eux
set -o pipefail

HOMEBREW_NO_AUTO_UPDATE=1 brew install \
    boost \
    doxygen \
    ninja