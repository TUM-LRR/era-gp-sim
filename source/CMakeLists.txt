########################################
# CONFIG
########################################

add_compile_options(-Os -O3 -g -Werror)

########################################
# SUBDIRECTORIES
########################################

add_subdirectory(arch)
add_subdirectory(common)
add_subdirectory(core)
