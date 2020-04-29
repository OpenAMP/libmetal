collect (PROJECT_LIB_HEADERS sys.h)

collect (PROJECT_LIB_SOURCES irq.c)
collect (PROJECT_LIB_SOURCES sys.c)

add_subdirectory(../xtensa_common ${CMAKE_CURRENT_BINARY_DIR}/../xtensa_common)
