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

add_library(erasim-common STATIC ${COMMON_SOURCES})

target_link_libraries(erasim-common erasim-core)
