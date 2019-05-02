#!/bin/bash -e
export PS4="$ "
set -x

if [ "$ENGINE" == "make" ]; then
	make lib
	sudo -E make install-so
	make exe
	sudo -E make install
elif [ "$ENGINE" == "meson" ]; then
	if [ "$TRAVIS_OS_NAME" != "windows" ]; then
		CC="$CC_" CXX="$CXX_" meson build --prefix=$HOME/.local
	else
		unset CC CXX CC_FOR_BUILD CXX_FOR_BUILD
		./meson.bat
	fi
	cd build
	ninja
fi
