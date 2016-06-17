########################################
# SETUP
########################################

cmake_minimum_required(VERSION 3.2)
set(CMAKE_BUILD_TYPE_INIT "Debug")

project(era-sim)

########################################
# DEPENDENCIES
########################################

#find_package(PkgConfig REQUIRED)

########################################
# INCLUDES
########################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

########################################
# SUBDIRECTORIES
########################################

add_subdirectory(source)

########################################
# TARGET
########################################

add_executable(era-sim main.cpp)

target_link_libraries(era-sim era-sim-arch)
target_compile_options(era-sim PUBLIC "-std=c++14")
