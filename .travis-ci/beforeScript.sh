#!/bin/bash -ev

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
	meson build
	cd build
	ninja
	ninja install
fi
