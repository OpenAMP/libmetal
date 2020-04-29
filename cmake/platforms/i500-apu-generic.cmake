set (CMAKE_SYSTEM_PROCESSOR "xtensa"              CACHE STRING "")
set (MACHINE                "i500_apu"            CACHE STRING "")
set (CROSS_PREFIX           "xtensa-unknown-elf-" CACHE STRING "")

option (CORE_ID "Build libmetal for a specific core" )
if (NOT DEFINED CORE_ID)
  set (CORE_ID "0")
endif (NOT DEFINED CORE_ID)
message ("-- Building libmetal for i500 APU core ${CORE_ID}")

set (CMAKE_C_FLAGS          "-DI500_CORE_ID=${CORE_ID}" CACHE STRING "")

include (cross-generic-gcc)
