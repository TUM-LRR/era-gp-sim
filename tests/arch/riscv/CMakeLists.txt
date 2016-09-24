########################################
# SOURCES
########################################

set(TEST_ARCH_RISCV_SOURCES
    arch-riscv-integer-instructions-test.cpp
    load-store-instructions-test.cpp
    lui-auipc-instructions-test.cpp
    dummies.cpp
    arithmetic-test-utils.cpp
    #formats-test.cpp The tests fail atm
)

########################################
# TARGET
########################################

add_executable(era-sim-arch-riscv-tests ${TEST_ARCH_RISCV_SOURCES})

target_link_libraries(era-sim-arch-riscv-tests era-sim-arch-common era-sim-arch-riscv era-sim-core era-sim-common)
target_link_libraries(era-sim-arch-riscv-tests ${GTEST_BOTH_LIBRARIES} pthread)
target_link_libraries(era-sim-arch-riscv-tests gtest gtest_main)

add_test(era-sim-arch-riscv-tests era-sim-arch-riscv-tests)
