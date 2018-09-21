#!/bin/bash -e
unset PS4
set -x

if [ "$ENGINE" == "make" ]; then
	make -C crunch lib
	sudo -E make -C crunch install-so
	make -C crunch++ lib
	sudo -E make -C crunch++ install-so
	make -C crunch crunch
	make -C crunch++ crunch++
	make -C crunchMake
	sudo -E make install
elif [ "$ENGINE" == "meson" ]; then
	CC="$CC_" CXX="$CXX_" meson build
	cd build
	ninja
	ninja install
fi
