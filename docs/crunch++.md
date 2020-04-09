# `crunch++` User Guide

Table of Contents

1. [Basic `crunch++` Usage](#basic-crunch++-usage)
	1. [Writing a Simple Test Suite](#writing-a-simple-test-suite)
	2. [Writing a Test Case](#writing-a-test-case)
	3. [Conditionally Skipping Tests and Suites](#conditionally-skipping-tests-and-suites)
2. [`crunch++` Assertions Reference](#crunch++-assertions-reference)
3. Getting the Most Out of `crunchMake` for `crunch++` Suites

## Basic `crunch++` usage

### Writing a Simple Test Suite

A `crunch++` test suite is a C++ file compiled using `crunchMake`.
`crunchMake` will automatically build the suite against `crunch++` when it detects a C++ file extension such as .cxx or .cpp.

A valid, albeit empty, suite takes the following basic form:

``` C++
#include <crunch++.h>

class testSuite final : public testsuit
{
public:
	void registerTests() final override { }
};

CRUNCHpp_TEST void registerCXXTests();
void registerCXXTests() { registerTestClasses<testSuite>(); }
```

With this, your suite should build and link, and `crunch++` should detect the presense of a valid suite in the resulting library and be able to run it.

To test this, save the above snippet as `test.cxx`, and run the following:

``` shell
crunchMake test.cxx
crunch++ test
```

The result should be:

``` shell
$ crunchMake test.cxx
c++ -fPIC -DPIC -fvisibility=hidden -fvisibility-inlines-hidden -std=c++11  test.cxx -shared -I/usr/include -L/usr/lib -Wl,-rpath,/usr/lib -lcrunch++ -O2 -pthread -o test.so

$ crunch++ test
Running test suite test...
Running tests in class 9testSuite...
Total tests: 0,  Failures: 0,  Pass rate: --
```

### Writing a Test Case

Building on the previous section, we can easily expand this stub suite to contain a test with a simple assertion.

Within the type `testSuite`, we can start by declaring a private section and a function that takes no arguments and returns nothing, just before the public section. Additionally, the function may call any code it likes, can make test assertions and can pass the `this` pointer for the suite to any function, as normal:

``` C++
private:
	void testCase()
	{
		assertNotNull(this);
		assertTrue(true);
	}
```

We also need to register the new test case using the registerTests function on the suite:

``` C++
	void registerTests() final override
	{
		CXX_TEST(testCase)
	}
```

With this added, rebuild and re-run the test suite:

``` shell
$ crunchMake test.cxx
c++ -fPIC -DPIC -fvisibility=hidden -fvisibility-inlines-hidden -std=c++11  test.cxx -shared -I/usr/include -L/usr/lib -Wl,-rpath,/usr/lib -lcrunch++ -O2 -pthread -o test.so

$ crunch++ test
Running test suite test...
Running tests in class 9testSuite...
testCase...                                                                          [  OK  ]
Total tests: 1,  Failures: 0,  Pass rate: 100.00%
```

### Conditionally Skipping Tests and Suites

`crunch++` allows us to do run-time detection that a test or suite does not apply or meet its run conditions (for example, because of missing environment variables) and provides a simple mechanism to do this.

Building on the suite so far, we can demonstrate this by adding a second test case to the existing suite, and defining a second suite in the same file. Add the following just after the end of `testCase()`:

``` C++
	void testSkip()
	{
		skip("Intentionally skipping this test");
	}
```

Register the test:

``` C++
	[...]
	{
		CXX_TEST(testCase)
		CXX_TEST(testSkip)
	}
```

We can then define a second suite in the same file, add the following after the end of the first suite's class definition:

``` C++
class testSkipSuite final : public testsuit
{
public:
	void registerTests() final override
	{
		skip("Intentionally skipping this suite");
	}
};
```

With this, the suite registration can be modified to register this suite too as follows:

``` C++
void registerCXXTests()
{
	registerTestClasses<testSuite, testSkipSuite>();
}
```

Building and re-running the suite should result in this:

``` shell
$ crunchMake test.cxx
c++ -fPIC -DPIC -fvisibility=hidden -fvisibility-inlines-hidden -std=c++11  test.cxx -shared -I/usr/include -L/usr/lib -Wl,-rpath,/usr/lib -lcrunch++ -O2 -pthread -o test.so

$ crunch++ test
Running test suite test...
Running tests in class 9testSuite...
testCase...                                                                          [  OK  ]
testSkip...
Skipping: Intentionally skipping this test                                           [ SKIP ]
Running tests in class 13testSkipSuite...
Skipping: Intentionally skipping this suite                                          [ SKIP ]
Total tests: 3,  Failures: 0,  Pass rate: 100.00%
```

## `crunch++` Assertions Reference

The equality assertions exist to remove the dependency on possibly overriden quality operators, which have the potential if themselves left untested to introduce errors and false assertion results into a test suite. This is especially true if your test requires a lot of external library headers.

`crunch++` comes with two kinds of afirmative equality assertion - fundamental pointer traits and general value assertions - and two boolean equality assertions.

### Positive Equality Assertions

* `assertNull` - Checks that the provided pointer, regardless of const-ness, is equivilent to a nullptr.
* `assertEqual` - Checks that two entities have the same value.

`assertNull` takes one parameter, the pointer to check. It provides two overloads, one to handle non-const pointers and the other to handle const pointers.

There are several forms of `assertEqual` check: Trivial value assertions for integers, C strings, characters, floating point values and direct address comparisons; and a more complex kind of assertion for validating memory contents.

The trivial form of `assertEqual` takes two parameters - `result`, and `expected` - in that order.
When the assertion fails, it print a diagnostic and aborts the test case.

The complex form of `assertEqual` has the signature

``` C++
void assertEqual(const void *const result, const void *const expected, const size_t expectedLength);
```

This allows for safe comparison of two blocks of memory, so allowing arbitrary object comparisons.
When the two memory blocks have different contents, the assertion fails and prints a diagnostic, aborting the test.

### Negative Equality Assertions

* `assertNotNull` - Checks that the provided pointer, regardless of const-ness is not equivilent to nullptr.
* `assertNotEqual` - Checks that one entity's value is not the same as the other.

`assertNotNull` takes one parameter, the pointer to check. It provides two overloads, on to handle non-const pointers and the other to handle const pointers.

There are several forms of `assertNotEqual` check: Trivial value assertions for integers, C strings, characters, floating point values and direct address comparisons; and a more complex kind of assertion for validating memory contents.

The trivial form of `assertNotEqual` takes two parameters - `result`, and `expected` - in that order.
If the two values are the same, the assertion fails and prints a diagnostic, aborting the test case.

The complex form of `assertNotEqual` has the signature

``` C++
void assertNotEqual(const void *const result, const void *const expected, const size_t expectedLength);
```

This allows for save comparison of two blocks of memory, as with `assertEqual`.
If the two blocks have identicle contents, the assertion fails and prints a diagnostic, aborting the test.

### Boolean Equality Assertions

* `assertTrue`
* `assertFalse`

Each of these takes a boolean-converted value and checks it holds the value given by the function's name.
If the value checked evaluates to something other than the desired boolean value, the assertion fails printing a diagnostic and aborting the test case.
