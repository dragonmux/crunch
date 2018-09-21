#!/bin/bash -e
unset PS4
set -x

if [ "$ENGINE" == "make" ]; then
	make test
	make check
elif [ "$ENGINE" == "meson" ]; then
	ninja test
fi
