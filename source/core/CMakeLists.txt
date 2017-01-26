########################################
# SOURCES
########################################

set(CORE_SOURCES
  memory-value.cpp
  register-set.cpp
  conversions.cpp
  project.cpp
  project-module.cpp
  parsing-and-execution-unit.cpp
  memory.cpp
  scheduler.cpp
  condition-timer.cpp
  snapshot.cpp
)

########################################
# TARGET
########################################

if (ERASIM_BUILD_CORE)
  add_library(erasim-core STATIC ${CORE_SOURCES})

  target_link_libraries(erasim-core erasim-common erasim-parser-common erasim-parser-factory erasim-arch-common)
  target_link_libraries(erasim-core pthread)
endif()
