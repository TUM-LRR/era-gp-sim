########################################
# SOURCES
########################################

set(ARCH_COMMON_SOURCES
  abstract-syntax-tree-node.cpp
  architecture-brewery.cpp
  architecture.cpp
  architecture-formula.cpp
  constituent-information.cpp
  datatype-information.cpp
  extension-information.cpp
  immediate-node.cpp
  information-interface.cpp
  instruction-information.cpp
  instruction-key.cpp
  instruction-set.cpp
  node-factory-collection.cpp
  node-factory-collection-maker.cpp
  register-information.cpp
  register-node.cpp
  unit-information.cpp
  validation-result.cpp
  constituent-information.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-arch-common STATIC ${ARCH_COMMON_SOURCES})

qt5_use_modules(era-sim-arch-common Widgets)
#target_link_libraries(era-sim-arch-common Qt5::Core)

target_link_libraries(era-sim-arch-common era-sim-common era-sim-arch-riscv)
