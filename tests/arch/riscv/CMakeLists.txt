########################################
# SOURCES
########################################

set(TEST_ARCH_RISCV_SOURCES
  arch-riscv-integer-instructions-test.cpp
  load-store-instructions-test.cpp
  lui-auipc-instructions-test.cpp
  rv64-only-instructions-test.cpp
  arithmetic-test-utils.cpp
  mul-div-instruction-tests.cpp
  formats-test.cpp
  arch-riscv-jump-instruction-tests.cpp
  arch-riscv-branch-instruction-tests.cpp
)

########################################
# TARGET
########################################

add_executable(era-sim-arch-riscv-tests ${TEST_ARCH_RISCV_SOURCES})

target_link_libraries(era-sim-arch-riscv-tests era-sim-arch-common era-sim-arch-riscv era-sim-core era-sim-common)
target_link_libraries(era-sim-arch-riscv-tests ${GTEST_BOTH_LIBRARIES})
target_link_libraries(era-sim-arch-riscv-tests gtest gtest_main)

add_test(era-sim-arch-riscv-tests era-sim-arch-riscv-tests)
