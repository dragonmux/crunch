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
		echo "    Program Files"
		ls '/c/Program Files (x86)'
		echo "    MSVC"
		ls '/c/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools/VC/Tools/MSVC/14.16.27023'
		echo "    Tools"
		ls '/c/Program Files (x86)/Microsoft Visual Studio 14.0/Tools'
		set -e
		# TODO: Source/run the VS2017 vcvarsall.bat
		meson build --prefix=$HOME/local
	fi
	cd build
	ninja
fi
