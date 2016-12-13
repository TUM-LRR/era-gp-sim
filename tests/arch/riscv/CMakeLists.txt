########################################
# SOURCES
########################################

set(TEST_ARCH_RISCV_SOURCES
  arithmetic-test-utils.cpp
  branch-instruction-test.cpp
  format-test.cpp
  integer-instruction-test.cpp
  jump-instruction-test.cpp
  load-store-instruction-test.cpp
  lui-auipc-instruction-test.cpp
  mul-div-instruction-test.cpp
  rv64-only-instruction-test.cpp
  register-test.cpp
  assemble-test.cpp
  simulator-instructions-test.cpp
  pseudo-instruction-test.cpp
  context-information-test.cpp
)

########################################
# TARGET
########################################

if (ERA_SIM_BUILD_ARCH)
  add_executable(era-sim-arch-riscv-tests ${TEST_ARCH_RISCV_SOURCES})

  target_link_libraries(era-sim-arch-riscv-tests era-sim-arch-riscv)
  target_link_libraries(era-sim-arch-riscv-tests gtest gtest_main)

  add_test(
    NAME era-sim-arch-riscv-tests
    COMMAND era-sim-arch-riscv-tests
    WORKING_DIRECTORY ${ERA_SIM_BINARY_DIR}
  )
endif()
