#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset
# set -o xtrace

git clone git://git.buildroot.net/buildroot /buildroot

cd /buildroot
git checkout -b csel 2021.11.1

rsync -a /workspace/config/board/ /buildroot/board/
rsync -a /workspace/config/configs/ /buildroot/configs/

make csel_defconfig
