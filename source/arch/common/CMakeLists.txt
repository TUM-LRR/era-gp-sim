########################################
# SOURCES
########################################

set(ARCH_COMMON_SOURCES
  register-information.cpp
  datatype-information.cpp
  unit-information.cpp
  extension-information.cpp
  architecture.cpp
  instruction-set.cpp
  instruction-information.cpp
  instruction-key.cpp
  information-interface.cpp
  architecture-brewery.cpp
  architecture-formula.cpp
  node-factory-collection.cpp
  node-factory-collection-maker.cpp
  constituent-information.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-arch-common STATIC ${ARCH_COMMON_SOURCES})

target_link_libraries(era-sim-arch-common era-sim-common era-sim-arch-riscv)
