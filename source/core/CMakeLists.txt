########################################
# SOURCES
########################################

set(CORE_SOURCES
  memory-value.cpp
  register-set.cpp
  conversions.cpp
  memory.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-core STATIC ${CORE_SOURCES})
