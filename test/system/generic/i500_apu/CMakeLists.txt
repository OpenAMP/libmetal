collect (PROJECT_LIB_TESTS helper.c)
collect (PROJECT_LIB_TESTS rsc_table.c)

set (_linker_script "${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/elf32xtensa.x")

set_property (GLOBAL PROPERTY TEST_LINKER_OPTIONS "-Wl,-build-id=none -specs=${CMAKE_CURRENT_SOURCE_DIR}/specs -T\"${_linker_script}\"")

collect(PROJECT_LIB_DEPS c)
collect(PROJECT_LIB_DEPS m)
