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

<<<<<<< HEAD
# For third-party includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
=======
FILE(GLOB_RECURSE HeaderFiles "include/*.hpp")
add_custom_target(headers SOURCES ${HeaderFiles})

>>>>>>> 324908ae885160a67a8d5b6d101c596aba618d3e
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

########################################
# SUBDIRECTORIES
########################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source)

########################################
# DEPENDENCIES
########################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party)

########################################
# TESTS
########################################

option(ERA_GP_SIM_TESTS "Enable tests" ON)

if(ERA_GP_SIM_TESTS)
  message(STATUS "Enabling and adding tests")
  enable_testing()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/googletest)
  add_subdirectory(tests)
endif()
