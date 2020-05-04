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
If the expression evalutes wrongly, then the assertion fails with a diagnostic, and aborts the test.

#### Integer Equality

There are several integer assertions for signed and unsigned at a couple of major sizes.

assertIntEqual and assertUintEqual perform standard integer assertions for signed and unsigned integers, respectively.
If the integers you wish to assert are smaller than this, these are still the functions to use as C's integer promotions will automatically bump the numbers up to this size by default.

assertInt64Equal and assertUint64Equal perform integer assertions of larger, 64-bit integers - both signed an unsigned, respectively.
These exist as C still "promotes" larger integers down to `int`, which is bad news for 64-bit numbers that are typically implemented using a type such as `long long`.

In all cases, if the two numbers are not equal, then the assertion fails with a diagnostic, and aborts the test.

#### Floating Point Equality

assertDoubleEqual asserts that two floating point numbers, possibly promoted floats, are equal. When they are not equal, this prints a diagnostic and fails, aborting the test.

#### String Equality

assertStringEqual asserts that two NUL terminated strings are equal. When they are not equal, this prints a diagnostic and fails, aborting the test.

#### Pointer Equality

There are two forms of pointer-based assertions.
assertPtrEqual which validates that two pointers point to the same place, and assertMemEqual which validates that the memory pointed to by two pointers has the same contents as each other.

assertMemEqual allows for safe comparison of two blocks of memory, so allowing arbitrary object comparisons.
When the two memory blocks do not have different contents, the assertion fails and prints a diagnostic, aborting the test case.

### Negative Equality Assertions

#### Integer Inequality

There are several integer assertions for signed and unsigned at a couple of major sizes.

assertIntNotEqual and assertUintNotEqual perform standard integer assertions for signed and unsigned integers, respectively.
If the integers you wish to assert are smaller than this, these are still the functions to use as C's integer promotions will automatically bump the numbers up to this size by default.

assertInt64NotEqual and assertUint64NotEqual perform integer assertions of larger, 64-bit integers - both signed an unsigned, respectively.
These exist as C still "promotes" larger integers down to `int`, which is bad news for 64-bit numbers that are typically implemented using a type such as `long long`.

In all cases, if the two numbers are equal then the assertion fails with a diagnostic, and aborts the test.

#### Floating Point Inequality

assertDoubleNotEqual asserts that two floating point numbers, possibly promoted floats, are not equal. When they are equal, this prints a diagnostic and fails, aborting the test.

#### String Inequality

assertStringNotEqual asserts that two NUL terminated strings are not equal. When they are equal, this prints a diagnostic and fails, aborting the test.

#### Pointer Inequality

There are two forms of pointer-based assertions.
assertPtrNotEqual which validates that two pointers do not point to the same place, and assertMemNotEqual which validates that the memory pointed to by two pointers does not have the same contents as one another.

assertMemEqual allows for safe comparison of two blocks of memory, so allowing arbitrary object comparisons.
When the two memory blocks have the same contents, the assertion fails and prints a diagnostic, aborting the test case.

## Getting the Most Out of `crunchMake` for `crunch` Suites
