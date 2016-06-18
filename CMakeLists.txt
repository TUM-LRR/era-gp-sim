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

add_subdirectory(source)

########################################
# TESTS
########################################

enable_testing()
add_subdirectory(tests)
