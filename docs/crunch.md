# `crunch` User Guide

Table of Contents

1. [Basic `crunch` Usage](#basic-crunch-usage)
	1. [Writing a Simple Test Suite](#writing-a-simple-test-suite)
	2. [Writing a Test Case](#writing-a-test-case)
2. [`crunch` Assertions Reference](#crunch-assertions-reference)
3. [Getting the Most Out of `crunchMake` for `crunch` Suites](#getting-the-most-out-of-crunchmake-for-crunch-suites)

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
  * [Integer Equality](#integer-equality)
  * [Floating Point Equality](#floating-point-equality)
  * [String Equality](#string-equality)
  * [Pointer Equality](#pointer-equality)
  * [Memory Equality](#memory-equality)
* [Negative Equality Assertions](#negative-equality-assertions)
  * [Integer Inequality](#integer-inequality)
  * [Floating Point Inequality](#floating-point-inequality)
  * [String Inequality](#string-inequality)
  * [Pointer Inequality](#pointer-inequality)
  * [Memory Inequality](#memory-inequality)
* [Boolean Equality Assertions](#boolean-equality-assertions)
* [Inequality Assertions](#inequality-assertions)

### Positive Equality Assertions

#### Integer Equality

There are several integer assertions for signed and unsigned at a couple of major sizes.

`assertIntEqual` and `assertUintEqual` perform standard integer assertions for signed and unsigned integers, respectively.
If the integers you wish to assert are smaller than this, these are still the functions to use as C's integer promotions will automatically bump the numbers up to this size by default.

`assertInt64Equal` and `assertUint64Equal` perform integer assertions of larger, 64-bit integers - both signed an unsigned, respectively.
These exist as C still "promotes" larger integers down to `int`, which is bad news for 64-bit numbers that are typically implemented using a type such as `long long`.

In all cases, if the two numbers are not equal, then the assertion fails with a diagnostic, and aborts the test case.

#### Floating Point Equality

`assertDoubleEqual` asserts that two floating point numbers, possibly promoted floats, are equal. When they are not equal, this prints a diagnostic and fails, aborting the test case.

#### String Equality

`assertStringEqual` asserts that two NUL terminated strings are equal. When they are not equal, this prints a diagnostic and fails, aborting the test case.

#### Pointer Equality

`assertPtrEqual` validates that two pointers point to the same place. When they do not, this prints a diagnostic and fails, aborting the test case.

`assertNull` and `assertConstNull` validate that a pointer is `NULL`.

NOTE: `assertConstNull` vs `assertNull` is rather historical and `assertConstNull` is deprecated and subject to removal in the next major release.

#### Memory Equality

`assertMemEqual` validates that the memory pointed to by two pointers has the same contents as each other.

`assertMemEqual` allows for safe comparison of two blocks of memory, so allowing arbitrary object comparisons.
When the two memory blocks do not have different contents, the assertion fails and prints a diagnostic, aborting the test case.

### Negative Equality Assertions

#### Integer Inequality

There are several integer assertions for signed and unsigned at a couple of major sizes.

`assertIntNotEqual` and `assertUintNotEqual` perform standard integer assertions for signed and unsigned integers, respectively.
If the integers you wish to assert are smaller than this, these are still the functions to use as C's integer promotions will automatically bump the numbers up to this size by default.

`assertInt64NotEqual` and `assertUint64NotEqual` perform integer assertions of larger, 64-bit integers - both signed an unsigned, respectively.
These exist as C still "promotes" larger integers down to `int`, which is bad news for 64-bit numbers that are typically implemented using a type such as `long long`.

In all cases, if the two numbers are equal then the assertion fails with a diagnostic, and aborts the test.

#### Floating Point Inequality

`assertDoubleNotEqual` asserts that two floating point numbers, possibly promoted floats, are not equal. When they are equal, this prints a diagnostic and fails, aborting the test.

#### String Inequality

`assertStringNotEqual` asserts that two NUL terminated strings are not equal. When they are equal, this prints a diagnostic and fails, aborting the test.

#### Pointer Inequality

`assertPtrNotEqual` validates that two pointers do not point to the same place. When they do, this prints a diagnostic and fails, aborting the test case.

`assertNotNull` and `assertConstNotNull` validate that a pointer is not `NULL`.

NOTE: `assertConstNotNull` vs `assertNotNull` is rather historical and `assertConstNotNull` is deprecated and subject to removal in the next major release.

#### Memory Inequality

`assertMemEqual` allows for safe comparison of two blocks of memory, so allowing arbitrary object comparisons.
When the two memory blocks have the same contents, the assertion fails and prints a diagnostic, aborting the test case.

### Boolean Equality Assertions

* `assertTrue`
* `assertFalse`

Each of these takes a truth value and checks it holds the value given by the function's name.
If the value checked evaluates to something other than the desired boolean value, the assertion fails printing a diagnostic and aborting the test case.diagnostic and aborting the test case.

### Inequality Assertions

* `assertGreaterThan` - Checks that `result` is greater than `expected`
* `assertGreaterThan64` - 64-bit variant which checks that `result` is greater than `expected`
* `assertLessThan` - Checks that `result` is less than `expected`
* `assertLessThan64` - 64-bit variant which checks that `result` is less than `expected`

These are integer-only assertions for performing range and broad value checks, aka inequalities.
These assertions take two parameters in order: `result` and `expected`.
On failure, these print a diagnostic and abort the test case.

## Getting the Most Out of `crunchMake` for `crunch` Suites

`crunchMake` is a tool that aims to ensure a working build of your tests without having to worry about exactly where crunch is installed or how it was built.

It provides transparency for many compiler options, and platform- and compiler-specific translations for the rest.

The important translated options when building crunch suites are:

* `--coverage` - This option enables the compiler-specific code coverage options for the build for when you
  do a code-coverage enabled build of your project
* `--debug` - This option enables debugging information on the test suite to allow setting breakpoints in
  the tests and inspecting state. Example usage of such a build: `gdb --args crunch testSuite`
