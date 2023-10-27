# Upstream repository for group projects (Autumn 2023)

Upstream repo for the group project

## Configuring the build

The first step is to create a subdirectory for the build.

> `mkdir build`

Then we run **CMake** in the `build` directory.

> `cd build`
> `cmake ..`

There are several options to `cmake` that you might want to specify:

* `-DCS237_VERBOSE_MAKEFILE=ON`

* `-DCS237_ENABLE_DOXYGEN=ON`

* `-DCMAKE_BUILD_TYPE=Release` specifies that a "release" build should
  be generated (the default is a "debug" build).  While a "release"
  build is faster (because there is less error checking), we recommend
  the debug build for its greater runtime error checking.

