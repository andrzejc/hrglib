#!/usr/bin/env bash
set -eux
set -o pipefail

HOMEBREW_NO_AUTO_UPDATE=1 brew install \
    boost \
    doxygen \
    ninja \
    ccache

export PATH="/usr/local/opt/ccache/libexec:$PATH"

unset -f cd
