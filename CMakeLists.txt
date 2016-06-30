########################################
# SETUP
########################################

cmake_minimum_required(VERSION 3.2)
set(CMAKE_BUILD_TYPE_INIT "Debug")

project(era-sim)

add_compile_options(-std=c++14)

########################################
# INCLUDES
########################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

########################################
# SUBDIRECTORIES
########################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source)

########################################
# DEPENDENCIES
########################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

########################################
# TESTS
########################################

option(ERA_GP_SIM_TESTS "Enable tests" ON)

if(ERA_GP_SIM_TESTS)
  message(STATUS "Enabling and adding tests")
  enable_testing()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest)
  add_subdirectory(tests)
endif()
