########################################
# SOURCES
########################################

set(TEST_ARCH_RISCV_SOURCES
	#arch-riscv-tests.cpp
	formats-test.cpp
)

########################################
# PACKAGES
########################################

#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

########################################
# TARGET
########################################

add_executable(era-sim-arch-riscv-tests ${TEST_ARCH_RISCV_SOURCES})

target_link_libraries(era-sim-arch-riscv-tests era-sim-arch-common era-sim-arch-riscv era-sim-core era-sim-common)
target_link_libraries(era-sim-arch-riscv-tests ${GTEST_BOTH_LIBRARIES} pthread)
target_link_libraries(era-sim-arch-riscv-tests gtest gtest_main)

add_test(era-sim-arch-riscv-tests era-sim-arch-riscv-tests)
