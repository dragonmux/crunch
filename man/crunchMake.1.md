% CRUNCHMAKE(1) crunch 1.0.0 | crunch unit testing framework

# NAME

**crunchMake** - `crunch` and `crunch++` test builder

# SYNOPSIS

| **crunchMake** \[**-h**|**\--help**]
| **crunchMake** \[**-v**|**\--version**]
| **crunchMake** \[_options_] _FILES_

# DESCRIPTION

`crunchMake` is a utility to build `crunch` and `crunch++` unit tests.

# OPTIONS

-v, \--version

:   Prints the version information for crunch

-h, \--help

:   Prints this help message

## Compiler support

-l**library**

:   Adds **library** to the compiler library linking set

-I**dir**

:   Adds **dir** to the compiler include search path

-D**macro**

:   Adds **macro** to the compiler's macro predefinitions

-L**dir**

:   Adds **dir** to the compiler library search path

-o **file**

:   Use **file** for the output test library

-pthread

:   Specify that you wish to build and link against pthreads

-Wl**option**

:   Adds **option** to the compiler-handled linker options

-std=**standard**

:   Sets the C or C++ standard to **standard**.
    In C++ mode, the minimimum supported standard is C++11.
    In C mode, the minimum supported standard is C89.

    This option must be specified in the form of either
    c**NN** or c++**NN** where **NN** is the version number to use.

-z **keyword**

:   Specifies direct linker options - the exact meaning and
    options available depend on your compiler's specific linker.

\--coverage

:   Enables linking against code that has been code-coverage enabled

\--debug

:   Specifies that you wish the test to be built in debug mode
    crunchMake defaults to building tests with level 2
    (or equivilent) optimisations.

-fsanitize=**sanitizers**

:   Enable the comma separated list of sanitizers **sanitizers**
    on the test being built

## Utility output options

\--log

:   Tells the engine to log all test output to the file named

-s, \--silent

:   Silences all output from crunchMake, leaving only compiler
    output on stdout and stderr

-q, \--quiet

:   Reduces output from crunchMake to kernel Makefile like output
    such as ' CCLD  test.cxx => test.so'

	By default, the utility will verbosely dump the full compiler invocations
    it is running same as typical `make` style utilities.

| The files given on the crunchMake command line are gathered together and passed as inputs to the compiler.
| This allows you to specify multiple TUs and additional object files to be compiled and linked to the suite to produce a complete library

# BUGS

Report bugs using [https://github.com/DX-MON/crunch/issues](https://github.com/DX-MON/crunch/issues)

# AUTHORS

Rachel Mant <dx-mon@users.sourceforge.net>

# SEE ALSO

**`crunch`(1)**, **`crunch++`(1)**
