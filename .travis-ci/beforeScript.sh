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
	[ "$TRAVIS_OS_NAME" == "windows" -a "$CC_" == "clang" ] && EXTRA_OPTS="-Dcpp_std=c++14" || EXTRA_OPTS=""
	[ $COVERAGE -ne 0 ] && EXTRA_OPTS="$EXTRA_OPTS --buildtype=debug"
	if [ "$TRAVIS_OS_NAME" != "windows" -o "$CC_" == "clang" ]; then
		CC="$CC_" CXX="$CXX_" meson build --prefix=$HOME/.local -Db_coverage=`codecov` $EXTRA_OPTS
		cd build
		ninja
	else
		unset CC CXX CC_FOR_BUILD CXX_FOR_BUILD
		.travis-ci/build.bat $ARCH `codecov`
	fi
fi

unset -f codecov
