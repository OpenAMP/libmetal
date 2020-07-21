# Copyright (c) 2020 Bobby Noelte
# SPDX-License-Identifier: BSD-3-Clause

# Build the OpenAMP libmetal hardware abstraction library as a Zephyr module.
#
# Zephyr does have it's own fork of libmetal which is enabled by CONFIG_LIBMETAL.

if (CONFIG_LIBMETAL_BY_OPENAMP)

    # Pathes
    # ------
    get_filename_component (LIBMETAL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
    get_filename_component (LIBMETAL_DIR_NAME ${LIBMETAL_ROOT_DIR} NAME_WE)
    set (LIBMETAL_BIN_ROOT "${CMAKE_BINARY_DIR}/modules/${LIBMETAL_DIR_NAME}")

    # Set Zephyr module environment (options)
    # ---------------------------------------
    set (PROJECT_NAME "metal")
    set (CMAKE_SYSTEM_NAME "Generic" CACHE STRING "")
    string (TOLOWER "Zephyr" PROJECT_SYSTEM)
    string (TOUPPER "Zephyr" PROJECT_SYSTEM_UPPER)
    if (CONFIG_CPU_CORTEX_M)
        set (MACHINE "cortexm" CACHE STRING "")
    endif (CONFIG_CPU_CORTEX_M)

    # Prevent generation of libraries - we do it on our own
    set (WITH_SHARED_LIB false)
    set (WITH_STATIC_LIB false)

    include ("${LIBMETAL_ROOT_DIR}/cmake/options.cmake")
    include ("${LIBMETAL_ROOT_DIR}/cmake/collect.cmake")

    # Just for info in case somebody needs it
    set (LIBMETAL_LIB_NAME ${PROJECT_NAME} CACHE STRING "")
    set (LIBMETAL_VERSION_MAJOR ${PROJECT_VER_MAJOR} CACHE STRING "")
    set (LIBMETAL_VERSION_MINOR ${PROJECT_VER_MINOR} CACHE STRING "")

    add_subdirectory ("${LIBMETAL_ROOT_DIR}/lib" "${LIBMETAL_BIN_ROOT}/lib")

    # Get collected library information
    # ---------------------------------
    collector_list (_sources PROJECT_LIB_SOURCES)
    foreach (_source ${_sources})
        if (NOT IS_ABSOLUTE ${_source})
           set (_source "${LIBMETAL_ROOT_DIR}/lib/${_source}")
        endif ()
        list (APPEND LIBMETAL_LIB_SOURCES ${_source})
    endforeach ()

    # Create the Zephyr library
    # -------------------------
    zephyr_library_named(${LIBMETAL_LIB_NAME})
    add_dependencies(${ZEPHYR_CURRENT_LIBRARY} ${OFFSETS_H_TARGET})
    zephyr_include_directories("${LIBMETAL_BIN_ROOT}/lib/include")
    zephyr_library_compile_options("-DMETAL_INTERNAL")
    zephyr_library_sources(${LIBMETAL_LIB_SOURCES})

endif ()