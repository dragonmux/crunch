#!/bin/bash -e
unset PS4
set -x

if [ "$ENGINE" == "meson" ]; then
	pyenv versions
	echo $PATH
	apt-cache search ninja-build
	pip3 install --user meson
fi
