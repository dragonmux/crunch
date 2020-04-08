# Building and Installing `crunch`

This will take you through the process of building `crunch` from source, then installing that build or, if you prefer to use a pre-built binary package, how to install one suitable to your distro of choice.

## Building `crunch` from source

### Prerequisites

`crunch` requires the following prerequisites that can be found in most distro package managers or your nearest Python package outlet, PyPI:

* `meson` >= 0.52 - The popular build system written in Python
* `ninja` >= 1.8.2 - The popular `make`-replacing parallel build tool

Additionally, the library requires you use a modern compiler that understands and is standards compliant with C++11.
For example, GCC >= 5, Clang >= 5 or MSVC 2019.

### The build

The [`meson` documentation](https://mesonbuild.com/) is fantastic, so to understand how `meson configure` works, please refer to that, but in a nutshell:

``` bash
meson build
cd build
ninja
```

or, if you prefer not to decend into the build directory:

``` bash
meson build
ninja -C build
```

## Installing `crunch` from source

With a build complete, simply run `ninja install` in the build directory or using `-C` syntax as above to specify the build directory to run the action in. You will be prompted via policykit for your password if appropriate to allow `ninja` to elevate privilages.

## Installing `crunch` from package management

[TBC]
