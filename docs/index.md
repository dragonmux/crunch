# crunch

Welcome to the documentation for the C and C++ unit testing framework crunch.

## Status

[![Build Status](https://travis-ci.org/DX-MON/crunch.svg?branch=master)](https://travis-ci.org/DX-MON/crunch)
[![codecov](https://codecov.io/gh/DX-MON/crunch/branch/master/graph/badge.svg)](https://codecov.io/gh/DX-MON/crunch)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/DX-MON/crunch.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/DX-MON/crunch/alerts/)
[![Coverity Status](https://scan.coverity.com/projects/20294/badge.svg)](https://scan.coverity.com/projects/dx-mon-crunch)

## Motivation

Having looked at and messed a bit with testing systems such as Deja GNU, Catch2 and others, I dedcided I did not like them nor how they would force me to work.

I went about building my own - one which coped with threads nicely, properly handles exceptions without aborting, has a simple automatic test registration system, did not require gratuitous preprocessor magic, and does not depend on nominating a TU to become 'main'.

While a testing framework such as Catch2 does handle exceptions reasonably, the vast majority hate threaded tests, are full of macro magic for test registration and assertions and spit out a single test binary that does all the work by default.

Why not make the tests shared libraries and move most of the common test assertion and registration logic into a global test runner and shared library? And that's just what crunch and crunch++ does.
