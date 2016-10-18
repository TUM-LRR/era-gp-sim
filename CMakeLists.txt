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
    set(CMAKE_CXX_STANDARD 14)
else()
    message(FATAL_ERROR "21st century, anyone?")
endif()

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

include_directories(/usr/local/opt/qt5/include/)

########################################
# INCLUDES
########################################

# For third-party includes (need relative to root)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${GTEST_INCLUDE_DIRS})

########################################
# SUBDIRECTORIES
########################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source)

########################################
# TESTS
########################################

option(ERA_GP_SIM_TESTS "Enable tests" ON)

if(ERA_GP_SIM_TESTS)
  message(STATUS "Enabling and adding tests")
  enable_testing()
  add_subdirectory(tests)
endif()
