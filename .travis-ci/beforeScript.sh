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
		echo "    MSVC"
		ls '/c/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools'
		set -e
		cat '/c/Program Files (x86)/Microsoft Visual Studio 14.0/Common7/Tools/vcvarsqueryregistry.bat'
		# TODO: Source/run the VS2017 vcvarsall.bat
		meson build --prefix=$HOME/local
	fi
	cd build
	ninja
fi
