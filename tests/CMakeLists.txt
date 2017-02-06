########################################
# CONFIG
########################################

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/std:c++latest /bigobj)
else()
    add_compile_options(-g -Werror -DDEBUG)
endif()

########################################
# SUBDIRECTORIES
########################################

add_subdirectory(arch)
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(parser)
add_subdirectory(system)
