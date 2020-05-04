# `crunch` User Guide

Table of Contents

1. [Basic `crunch` Usage](#basic-crunch-usage)
	1. [Writing a Simple Test Suite](#writing-a-simple-test-suite)
	2. [Writing a Test Case](#writing-a-test-case)
2. [`crunch` Assertions Reference](#crunch-assertions-reference)
3. [Getting the Most Out of `crunchMake` for `crunch` Suites](getting-the-most-out-of-crunchmake-for-crunch-suites)

## Basic `crunch++` usage

### Writing a Simple Test Suite

A `crunch` test suite is a C file compiled using `crunchMake`.
`crunchMake` will automatically build the suite against `crunch` when it detects the C file extension .c.

Please note, for this purpose, '.C' is also considered a C extension as the utility considers this equal to .c on account of DOS, FAT32 and other case-insensitive file systems historically and generally found in use.

A valid, albeit empty, suite takes the following basic form:

``` C
#include <crunch.h>

BEGIN_REGISTER_TESTS()
END_REGISTER_TESTS()
```

With this, your suite should build and link, and `crunch` should detect the presense of a valid suite in the resulting library and be able to run it.

To test this, save the above snippet as `test.c`, and run the following:

``` shell
crunchMake test.c
crunch test
```

The result should be:

``` shell
$ crunchMake test.c
cc -fPIC -DPIC  test.c -shared -I/usr/include -L/usr/lib -Wl,-rpath,/usr/lib -lcrunch -O2 -pthread -o test.so

$ crunch test
Running test suit test...
Total tests: 0,  Failures: 0,  Pass rate: --
```

### Writing a Test Case

Building on the previous section, we can easily expand this stub suite to contain a test with a simple assertion.

Before the BEGIN_REGISTER_TESTS() block, we can declare a function that takes no arguments and returns nothing. Additionally, the function may call any code it likes, can make test assertions and any code it calls can too if it includes the crunch header:

``` C
#include <stdbool.h>

void testCase()
{
	assertNull(NULL);
	assertTrue(true);
}
```

We also need to register the new test case using the registerTests function on the suite:

``` C
BEGIN_REGISTER_TESTS()
	TEST(testCase)
END_REGISTER_TESTS()
```

With this added, rebuild and re-run the test suite:

``` shell
$ crunchMake test.c
cc -fPIC -DPIC  test.c -shared -I/usr/include -L/usr/lib -Wl,-rpath,/usr/lib -lcrunch -O2 -pthread -o test.so

$ crunch test
Running test suit test...
testCase...                                                                          [  OK  ]
Total tests: 1,  Failures: 0,  Pass rate: 100.00%
```

## `crunch` Assertions Reference

`crunch` comes with two kinds of affirmative equality assertion - fundamental pointer traits and general value assertions - and two boolean equality assertions.

Reference Table of Contents

* [Positive Equality Assertions](#positive-equality-assertions)
* [Negative Equality Assertions](#negative-equality-assertions)
* [Boolean Equality Assertions](#boolean-equality-assertions)
* [Inequality Assertions](#inequality-assertions)

### Positive Equality Assertions

#### Boolean Equality

There are two boolean assertions - assertTrue and assertFalse.
These take an expression evaluating to a truth value, and perform an assertion that the expression is either true or false (respectively).

#### Integer Equality

There are several integer assertions for signed and unsigned at a couple of major sizes.

assertIntEqual and assertUintEqual perform standard integer assertions for signed and unsigned integers, respectively.
If the integers you wish to assert are smaller than this, these are still the functions to use as C's integer promotions will automatically bump the numbers up to this size by default.

assertInt64Equal and assertUint64Equal perform integer assertions of larger, 64-bit integers - both signed an unsigned, respectively.
These exist as C still "promotes" larger integers down to `int`, which is bad news for 64-bit numbers that are typically implemented using a type such as `long long`.

## Getting the Most Out of `crunchMake` for `crunch` Suites
