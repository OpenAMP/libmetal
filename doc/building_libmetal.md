# Building libmetal

libmetal uses a CMake based build setup.

## Project configuration

The configuration phase begins when the user invokes CMake. CMake begins by
processing the CMakeLists.txt file and the cmake directory. Some cmake options
are available to help user to customize the libmetal to their own project.

* **WITH_DOC** (default ON): Build with documentation. Add -DWITH_DOC=OFF in
cmake command line to disable.
* **WITH_TESTS** (default ON): Build with application tests. Add -DWITH_DOC=OFF
in cmake command line to disable the option.
* **WITH_DEFAULT_LOGGER** (default ON): Build with default trace logger. Add
-DWITH_DEFAULT_LOGGER=OFF in cmake command line to disable the option.
* **WITH_SHARED_LIB** (default ON): Generate a shared library. Add
-DWITH_SHARED_LIB=OFF in cmake command line to disable the option.
* **WITH_STATIC_LIB** (default ON): Generate a static library. Add
-DWITH_STATIC_LIB=OFF in cmake command line to disable the option.
*  **WITH_ZEPHYR** (default OFF): Build for Zephyr environment. Add
-DWITH_ZEPHYR=ON in cmake command line to enable the the option.

## Build Steps

### Building for Linux Host
```shell
git clone https://github.com/OpenAMP/libmetal.git
mkdir -p libmetal/<build directory>
cd libmetal/<build directory>
cmake ..
make VERBOSE=1 DESTDIR=<libmetal install location> install
```

### Cross Compiling for Linux Target

Use [meta-openamp](https://github.com/openamp/meta-openamp) to build
libmetal library.

Use package `libmetal` in your Yocto config file.

### Building for Baremetal

To build on baremetal, you will need to provide a toolchain file. Here is an
example toolchain file:
```
    set (CMAKE_SYSTEM_PROCESSOR "arm"              CACHE STRING "")
    set (MACHINE "zynqmp_r5" CACHE STRING "")

    set (CROSS_PREFIX           "armr5-none-eabi-" CACHE STRING "")
    set (CMAKE_C_FLAGS          "-mfloat-abi=soft -mcpu=cortex-r5 -Wall -Werror -Wextra \
       -flto -Os -I/ws/xsdk/r5_0_bsp/psu_cortexr5_0/include" CACHE STRING "")

    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
    SET(CMAKE_AR  "gcc-ar" CACHE STRING "")
    SET(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
    SET(CMAKE_C_ARCHIVE_FINISH   true)

    include (cross-generic-gcc)
```
* Note: other toolchain files can be found in the  `cmake/platforms/` directory.
* Compile with your toolchain file.
```shell
mkdir -p build-libmetal
cd build-libmetal
cmake <libmetal_source> -DCMAKE_TOOLCHAIN_FILE=<toolchain_file>
make VERBOSE=1 DESTDIR=<libmetal_install> install
```
* Note: When building baremetal for Xilinx 2018.3 or earlier environments,
add -DXILINX_PRE_V2019 to your CMake invocation. This will include the
xilmem and xilstandalone libraries in your build. These libraries were
removed in 2019.1.

### Building for Zephyr

The [zephyr-libmetal](https://github.com/zephyrproject-rtos/libmetal)
implements the libmetal for the Zephyr project. It is mainly a fork of this
repository, with some add-ons for integration in the Zephyr project.

Following instruction is only to be able to run test application on a QEMU
running a Zephyr environment.

As Zephyr uses CMake, we build libmetal library and test application as targets
of Zephyr CMake project. Here is how to build libmetal for Zephyr:
```shell
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
export ZEPHYR_SDK_INSTALL_DIR=<where Zephyr SDK is installed>
source <git_clone_zephyr_project_source_root>/zephyr-env.sh

cmake <libmetal_source_root> -DWITH_ZEPHYR=on -DBOARD=qemu_cortex_m3 \
      [-DWITH_TESTS=on]
make VERBOSE=1 all
    # If we have turned on tests with "-DWITH_TESTS=on" when we run cmake,
    # we launch libmetal test on Zephyr QEMU platform as follows:
make VERBOSE=1 run
```
