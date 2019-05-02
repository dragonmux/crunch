#!/bin/bash -e
unset PS4
set -x

if [ "$TRAVIS_OS_NAME" == "windows" ]; then
	alias wget='wget --progress=dot:giga'
	python --version
	python3 --version
	wget https://www.python.org/ftp/python/3.6.8/python-3.6.8-amd64.exe
	wget https://bootstrap.pypa.io/get-pip.py
	wget https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-win.zip
	./python-3.6.8-amd64.exe /quiet PrependPath=1 Include_pip=0 SimpleInstall=1
	python get-pip.py --user
	mkdir /c/tools/ninja-build
	7z x -oC:\tools\ninja-build ninja-win.zip
	rm python-3.6.8-amd64.exe get-pip.py ninja-win.zip
elif [ "$ENGINE" == "meson" ]; then
	wget https://bootstrap.pypa.io/get-pip.py
	wget https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip
	python3.6 get-pip.py --user
	pip3 install --user meson
	unzip ninja-linux.zip -d ~/.local/bin
	rm get-pip.py ninja-linux.zip
fi
