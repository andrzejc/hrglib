#!/usr/bin/env bash
set -eux
set -o pipefail

sudo apt-get update -q
sudo apt-get install \
    python3.7 \
    ninja-build \
    doxygen \
    libboost-dev

sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.7 1
curl https://bootstrap.pypa.io/get-pip.py | sudo -H python3
