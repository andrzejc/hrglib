#!/usr/bin/env bash
set -eux
set -o pipefail

HOMEBREW_NO_AUTO_UPDATE=1 brew install \
    boost \
    doxygen \
    ninja \
    ccache

export PATH="/usr/local/opt/ccache/libexec:$PATH"

# XXX on osx Travis uses some ruby wrapper function around cd which has
# unbound variables and errors on our set -u; this is waste of CPU on
# non-interactive shell, so unset.
unset -f cd
