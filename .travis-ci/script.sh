#!/bin/bash -e
export PS4="$ "
set -x

ninja -C build test
ninja -C build install
