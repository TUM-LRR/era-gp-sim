########################################
# CONFIG
########################################

add_compile_options(-g -Werror -DDEBUG)

########################################
# SUBDIRECTORIES
########################################

add_subdirectory(arch)
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(parser)
