#!/bin/bash -e
unset PS4
set -x

if [ "$ENGINE" == "meson" ]; then
	pip3 install --user meson
	echo $PATH
	apt-cache search ninja-build
fi
