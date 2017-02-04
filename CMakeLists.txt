########################################
# SETUP
########################################

cmake_minimum_required(VERSION 3.2)
project(era-sim)

########################################
# C++ VERSIONING
########################################

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX_14)

if (COMPILER_SUPPORTS_CXX_14)
    set(CMAKE_CXX_STANDARD 14)
else()
    message(FATAL_ERROR "21st century, anyone?")
endif()

########################################
# CONFIG
########################################

# Specify default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
    "Available build types: Debug Release RelWithDebInfoMinSizeRel" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Interpret warnings as errors
add_compile_options(-Werror)

# Debug specific options
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

########################################
# DEPENDENCIES
########################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party)

# The following variables are for Qt support
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the actual Qt5 packages
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Svg REQUIRED)

########################################
# INCLUDES
########################################

# For third-party includes (need relative to root)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${GTEST_INCLUDE_DIRS})

########################################
# BINARIES
########################################

option(ERA_SIM_BUILD_ARCH "Enable compilation of arch modules" ON)
option(ERA_SIM_BUILD_PARSER "Enable compilation of parser modules" ON)
option(ERA_SIM_BUILD_CORE "Enable compilation of core modules" ON)
option(ERA_SIM_BUILD_GUI "Enable compilation of gui modules" ON)

set(ERA_SIM_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)
message(STATUS "Building to directory ${ERA_SIM_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ERA_SIM_BINARY_DIR})

########################################
# SUBDIRECTORIES
########################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source)

########################################
# TESTS
########################################

option(ERA_SIM_TESTS "Enable tests" ON)

if(ERA_SIM_TESTS)
  message(STATUS "Enabling and adding tests")
  enable_testing()
  add_subdirectory(tests)
endif()

########################################
# QTCREATOR
########################################

# Unfortunately this is needed for QtCreator (CMake-Project) to
# recognize all header files in the include directory
# Obviously DON'T build or link these created targets/libraries
FILE(GLOB_RECURSE QTCREATOR_INCLUDES "include/*")
add_custom_target(dummy-qtcreator-includes SOURCES ${QTCREATOR_INCLUDES})
add_library(dummy-qtcreator-lib EXCLUDE_FROM_ALL ${QTCREATOR_INCLUDES})


########################################
# CUSTOM TARGETS
########################################

# Creates symlinks from the binary directory to the resource directories
if (UNIX)
  add_custom_target(symlinks ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink
      ${CMAKE_SOURCE_DIR}/isa ${ERA_SIM_BINARY_DIR}/isa
    COMMAND ${CMAKE_COMMAND} -E create_symlink
      ${CMAKE_SOURCE_DIR}/themes ${ERA_SIM_BINARY_DIR}/themes
  )
endif(UNIX)
