########################################
# SOURCES
########################################

set(TEST_ARCH_RISCV_SOURCES
	arch-riscv-tests.cpp
)

########################################
# PACKAGES
########################################

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

########################################
# TARGET
########################################

add_executable(era-sim-arch-riscv-tests ${TEST_ARCH_RISCV_SOURCES})

target_link_libraries(era-sim-arch-riscv-tests era-sim-arch-common era-sim-arch-riscv)
target_link_libraries(era-sim-arch-riscv-tests ${GTEST_BOTH_LIBRARIES} pthread)

add_test(era-sim-arch-riscv-tests era-sim-arch-riscv-tests)
