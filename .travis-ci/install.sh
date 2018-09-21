#!/bin/bash -e
unset PS4
set -x

if [ "$ENGINE" == "meson" ]; then
	wget https://bootstrap.pypa.io/get-pip.py
	python3.6 get-pip.py --user
	pip3 install --user meson
	apt-cache show ninja-build
fi
