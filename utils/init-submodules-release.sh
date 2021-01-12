#!/bin/bash

source ./safeCommandsSet.sh

if git submodule status 2>&1 | grep -iq "fatal: Not a git repository (or any of the parent directories): .git"; then
    save_cd "$(dirname "$0")"/../external/
    git clone git@github.com:google/googletest.git
    save_cd googletest/
    git checkout release-1.8.0
    save_cd -
    git clone git@github.com:nlohmann/json.git
    save_cd json/
    git checkout v3.4.0
    save_cd -
    git clone https://github.com/pdschubert/WALi-OpenNWA.git
fi
