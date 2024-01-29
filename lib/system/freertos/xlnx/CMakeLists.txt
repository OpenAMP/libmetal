collect (PROJECT_LIB_HEADERS sys.h)

collect (PROJECT_LIB_SOURCES irq.c)
collect (PROJECT_LIB_SOURCES sys.c)

if (CMAKE_C_FLAGS MATCHES "SDT")
  collect (PROJECT_LIB_HEADERS sys_devicetree.h)
endif (CMAKE_C_FLAGS MATCHES "SDT")
