#!/bin/bash -e
export PS4="$ "
set -x

if [ "$ENGINE" == "make" ]; then
	make test
	make check
elif [ "$ENGINE" == "meson" ]; then
	cd build
	if [ "$TRAVIS_OS_NAME" != "windows" ]; then
		ninja test
	else
		meson test --no-rebuild --num-processes 1 -v
	fi
	ninja install
fi
