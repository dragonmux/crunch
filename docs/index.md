# [![Build Status](https://travis-ci.org/DX-MON/crunch.svg?branch=master)](https://travis-ci.org/DX-MON/crunch) [![codecov](https://codecov.io/gh/DX-MON/crunch/branch/master/graph/badge.svg)](https://codecov.io/gh/DX-MON/crunch) [![Total alerts](https://img.shields.io/lgtm/alerts/g/DX-MON/crunch.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/DX-MON/crunch/alerts/) [![Coverity Status](https://scan.coverity.com/projects/20294/badge.svg)](https://scan.coverity.com/projects/dx-mon-crunch)

crunch is a test suite framework designed to be simple and modular.
It allows for tests to be written simply and in a logical manner without any complicated setup or dependencies, and for tests to be easily run.

## Motivation

When looking at what the current C and C++ testing landscape had to offer, there was a clear favoritism to very macro heavy test suites and integrations. On top of this, most other solutions were needlessly complicated, often requiring explicit main methods to be defined in their own translation units with magic defines or macros.

Additionally, there was a seeming lack of threading-compatable frameworks and many had patchy support for proper arbitrary exception handling.
Most frameworks can't handle assertions on a threads other than the main test, and many crash when you throw an exception not derived from `std::exception`.
This has a large amount of room for improvement, which crunch provides.

By extracting the test runner logic out of the monolithic test binary that most testing frameworks produce, crunch allows for multiple smaller tests to be written without massive linking penalties that other frameworks come with.
Having the tests and the test runner separate also allows for crunch to handle multi-threaded tests and otherwise hard to catch exceptions with relative ease.
