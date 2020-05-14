% CRUNCH++(1) crunch 1.0.0 | crunch unit testing framework

# NAME

**crunch++** - C++ test harness and execution engine

# SYNOPSIS

| **crunch++** \[**-h**|**\--help**]
| **crunch++** \[**-v**|**\--version**]
| **crunch++** \[**\--log** _file_] _TESTS_

# DESCRIPTION

`crunch++` is the test harness and execution engine for C++ crunch tests.

The engine takes one or more unit test shared objects built using `crunchMake`
and loads, executes and reports the results from the suites contained within.

# OPTIONS

-v, \--version

:   Prints the version information for crunch

-h, \--help

:    Prints this help message

\--log

:   Tells the engine to log all test output to the file named

# BUGS

Report bugs using [https://github.com/DX-MON/crunch/issues](https://github.com/DX-MON/crunch/issues)

# AUTHORS

Rachel Mant <dx-mon@users.sourceforge.net>

# SEE ALSO

**`crunchMake`(1)**
