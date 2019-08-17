#!/bin/bash -e
export PS4="$ "
set -x

codecov()
{
	if [ $COVERAGE -ne 0 ]; then
		echo true
	else
		echo false
	fi
}

if [ "$ENGINE" == "make" ]; then
	make lib
	sudo -E make install-so
	make exe
	sudo -E make install
elif [ "$ENGINE" == "meson" ]; then
	EXTRA_OPTS=""
	[ "$TRAVIS_OS_NAME" == "windows" -a "$CC_" == "clang" ] && EXTRA_OPTS="-Dcpp_std=c++14"
	if [ "$TRAVIS_OS_NAME" != "windows" -o "$CC_" == "clang" ]; then
		CC="$CC_" CXX="$CXX_" meson build --prefix=$HOME/.local -D b_coverage=`codecov` $EXTRA_OPTS
		cd build
		ninja
	else
		unset CC CXX CC_FOR_BUILD CXX_FOR_BUILD
		.travis-ci/build.bat $ARCH `codecov`
	fi
fi

unset -f codecov
