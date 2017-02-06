########################################
# CONFIG
########################################

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/std:c++latest /bigobj)
endif()

########################################
# SUBDIRECTORIES
########################################

add_subdirectory(arch)
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(parser)
