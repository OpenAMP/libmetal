# libmetal

## Overview

Libmetal provides common user APIs to access devices, handle device interrupts
and request memory across the following operating environments:
  * Linux user space (based on UIO and VFIO support in the kernel)
  * RTOS (with and without virtual memory)
  * Bare-metal environments

For more details on the framework please refer to the
[OpenAMP Docs](https://openamp.readthedocs.io/en/latest/).

For details on the libmetal interfaces refer to
[Libmetal Interfaces](./doc/libmetal_interfaces.md).

## Building libmetal

libmetal uses a [CMake](https://cmake.org/) based build setup.

For embedded Linux a Yocto Project layer is provided as
[meta-openamp](https://github.com/openamp/meta-openamp).

The Zephyr Project has fully integrated libmetal as part of its open-amp subsystem implementation.
Refer to [Zephyr OpenAMP](https://docs.zephyrproject.org/latest/samples/subsys/ipc/openamp/README.html).

For more detailed build information refer to [Building libmetal](./doc/building_libmetal.md).

## Deprecated platforms
  - AMD-Xilinx Zynq7000. Last known stable release - v2023.04

## Contributing

For contribution refer to [Contribute to Libmetal](./doc/libmetal_contributions.md)

## Communication and Collaboration

[Subscribe](https://lists.openampproject.org/mailman3/lists/openamp-rp.lists.openampproject.org/)
to the OpenAMP mailing list (openamp-rp@lists.openampproject.org).
