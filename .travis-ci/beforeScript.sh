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

[ "$TRAVIS_OS_NAME" == "windows" -a "$CC" == "clang" ] && EXTRA_OPTS="-Dcpp_std=c++14" || EXTRA_OPTS=""
[ $COVERAGE -ne 0 ] && EXTRA_OPTS="$EXTRA_OPTS --buildtype=debug"

if [ "$TRAVIS_OS_NAME" != "windows" -o "$CC" == "clang" ]; then
	meson build --prefix=$HOME/.local -Db_coverage=`codecov` $EXTRA_OPTS
	ninja -C build
else
	unset CC CXX CC_FOR_BUILD CXX_FOR_BUILD
	.travis-ci/build.bat $ARCH `codecov`
fi

unset -f codecov
