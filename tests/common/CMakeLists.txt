########################################
# SOURCES
########################################

set(TEST_COMMON_SOURCES
  common-utility-tests.cpp
  multiregex-test.cpp
  common-assert-tests.cpp
  common-string-conversions-tests.cpp
  bit-builder-tests.cpp
)

########################################
# TARGET
########################################

add_executable(erasim-common-tests ${TEST_COMMON_SOURCES})

target_link_libraries(erasim-common-tests erasim-common)
target_link_libraries(erasim-common-tests gtest gtest_main)

add_test(
  NAME erasim-common-tests
  COMMAND erasim-common-tests
  WORKING_DIRECTORY ${ERASIM_BINARY_DIR}
)
