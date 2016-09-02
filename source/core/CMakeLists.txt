########################################
# SOURCES
########################################

set(CORE_SOURCES
  memory-value.cpp
  register-set.cpp
  project.cpp
  project-module.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-core STATIC ${CORE_SOURCES})
