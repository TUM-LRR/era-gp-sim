########################################
# SOURCES & HEADERS
########################################

set(UI_SOURCES
    main.cpp
    registeritem.cpp
    registermodel.cpp
    registerdata.cpp
    gui-project.cpp
    syntaxhighlighter.cpp
    editor-component.cpp
    ui.cpp
)

set(UI_HEADERS
    ../../include/ui/registeritem.hpp
    ../../include/ui/registermodel.hpp
    ../../include/ui/gui-project.hpp
    ../../include/ui/syntaxhighlighter.hpp
    ../../include/ui/registerdata.hpp
    ../../include/ui/editor-component.hpp
    ../../include/ui/ui.hpp
)

########################################
# EXECUTABLe
########################################

qt5_add_resources(UI_RESOURCES qml.qrc)
qt5_wrap_cpp(UI_HEADERS_WRAPPED ${UI_HEADERS})

add_executable(gui ${UI_SOURCES} ${UI_RESOURCES} ${UI_HEADERS_WRAPPED})

qt5_use_modules(gui Widgets Qml Quick)

target_link_libraries(gui era-sim-common era-sim-arch-common)
