########################################
# SOURCES & HEADERS
########################################

set(UI_SOURCES
    main.cpp
    register-model.cpp
    syntaxhighlighter.cpp
    editor-component.cpp
    clipboard-adapter.cpp
    memory-component-presenter.cpp
    ui.cpp
    gui-project.cpp
)

set(UI_HEADERS
    ../../include/ui/register-model.hpp
    ../../include/ui/gui-project.hpp
    ../../include/ui/syntaxhighlighter.hpp
    ../../include/ui/editor-component.hpp
    ../../include/ui/clipboard-adapter.hpp
    ../../include/ui/memory-component-presenter.hpp
    ../../include/ui/ui.hpp
)


########################################
# EXECUTABLE
########################################

qt5_add_resources(UI_RESOURCES qml.qrc)
qt5_wrap_cpp(UI_HEADERS_WRAPPED ${UI_HEADERS})

add_executable(gui ${UI_SOURCES} ${UI_RESOURCES} ${UI_HEADERS_WRAPPED})

target_link_libraries(gui era-sim-core)

qt5_use_modules(gui Widgets Qml Quick)
