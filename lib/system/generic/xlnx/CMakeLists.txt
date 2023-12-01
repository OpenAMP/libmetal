if ("${PROJECT_MACHINE}" STREQUAL "microblaze_generic")
  add_subdirectory(microblaze_generic)
else()
  collect (PROJECT_LIB_SOURCES sys.c)
endif()

collect (PROJECT_LIB_HEADERS sys.h)

collect (PROJECT_LIB_SOURCES irq.c)

include(CheckSymbolExists)
check_symbol_exists(SDT "bspconfig.h" HAS_SYSTEM_DT)
if (HAS_SYSTEM_DT)
  collect (PROJECT_LIB_HEADERS sys_devicetree.h)
endif()
