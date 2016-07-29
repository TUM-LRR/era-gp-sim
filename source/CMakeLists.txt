########################################
# CONFIG
########################################

add_compile_options(-Os -O2 -g -Werror)

########################################
# SUBDIRECTORIES
########################################

add_subdirectory(arch)
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(parser)
