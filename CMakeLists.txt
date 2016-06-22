cmake_minimum_required (VERSION 2.6)

list (APPEND CMAKE_MODULE_PATH
      "${CMAKE_SOURCE_DIR}/cmake"
      "${CMAKE_SOURCE_DIR}/cmake/modules"
      "${CMAKE_SOURCE_DIR}/cmake/platforms")

project (metal C)

include (CheckIncludeFiles)
include (CheckCSourceCompiles)
include (collect)
include (options)
include (depends)

enable_testing ()

add_subdirectory (lib)
if (WITH_TESTS)
  add_subdirectory (test)
endif (WITH_TESTS)
add_subdirectory (doc)

# vim: expandtab:ts=2:sw=2:smartindent
