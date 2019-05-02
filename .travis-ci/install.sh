#!/bin/bash -e
unset PS4
set -x

if [ "$TRAVIS_OS_NAME" == "windows" ]; then
	wget --progress=dot:mega https://bootstrap.pypa.io/get-pip.py
	wget --progress=dot:mega https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-win.zip
	choco install python --version 3.6.8
	python --version
	python get-pip.py --user
	pip3 install --user meson
	mkdir /c/tools/ninja-build
	7z x -oC:\tools\ninja-build ninja-win.zip
	rm get-pip.py ninja-win.zip
elif [ "$ENGINE" == "meson" ]; then
	wget https://bootstrap.pypa.io/get-pip.py
	wget https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip
	python3.6 get-pip.py --user
	pip3 install --user meson
	unzip ninja-linux.zip -d ~/.local/bin
	rm get-pip.py ninja-linux.zip
fi
