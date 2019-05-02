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
		set +e
		echo "    Tools"
		ls '/c/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools/Common7/Tools'
		set -e
		'/c/Program Files (x86)/Microsoft Visual Studio 14.0/Common7/Tools/vcvars32.bat'
		# TODO: Source/run the VS2017 vcvarsall.bat
		meson build --prefix=$HOME/local
	fi
	cd build
	ninja
fi
