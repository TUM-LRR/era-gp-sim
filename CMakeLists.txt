########################################
# SETUP
########################################

cmake_minimum_required(VERSION 3.2)
set(CMAKE_BUILD_TYPE_INIT "Debug")

project(era-sim)

########################################
# C++ Versioning
########################################

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX_14)

if (COMPILER_SUPPORTS_CXX_14)
   add_compile_options(-std=c++14)
else()
 message(FATAL_ERROR "21st century, anyone?")
endif()

########################################
# INCLUDES
########################################

# For third-party includes (need relative to root)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
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
