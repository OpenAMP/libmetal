collect (PROJECT_LIB_TESTS main.c)
collect (PROJECT_LIB_TESTS alloc.c)
if (WITH_IRQ_HANDLER)
  collect (PROJECT_LIB_TESTS irq.c)
endif (WITH_IRQ_HANDLER)
collect (PROJECT_LIB_TESTS mutex.c)
collect (PROJECT_LIB_TESTS atomic.c)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_MACHINE})
  add_subdirectory(${PROJECT_MACHINE})
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_MACHINE})

# vim: expandtab:ts=2:sw=2:smartindent
