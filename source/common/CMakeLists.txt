########################################
# SOURCES
########################################

set(COMMON_SOURCES
  utility.cpp
  string-conversions.cpp
  translateable.cpp
  status.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-common STATIC ${COMMON_SOURCES})

target_link_libraries(era-sim-common era-sim-core)
