########################################
# SUBDIRECTORIES
########################################

# Placeholder for the future

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/googletest)

set(GTEST_INCLUDE_DIRS
  ${gtest_SOURCE_DIR}/include
  ${gtest_SOURCE_DIR}
  PARENT_SCOPE)

# json is header-only and does not need a CMakeLists
