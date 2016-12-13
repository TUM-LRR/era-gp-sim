########################################
# SOURCES & HEADERS
########################################

set(UI_SOURCES
    main.cpp
    register-model.cpp
    syntaxhighlighter.cpp
    editor-component.cpp
    memory-component-presenter.cpp
    ui.cpp
    gui-project.cpp
    snapshot-component.cpp
    output-component.cpp
    translateable-processing.cpp
)

set(UI_HEADERS
    ../../include/ui/register-model.hpp
    ../../include/ui/gui-project.hpp
    ../../include/ui/syntaxhighlighter.hpp
    ../../include/ui/editor-component.hpp
    ../../include/ui/memory-component-presenter.hpp
    ../../include/ui/ui.hpp
    ../../include/ui/snapshot-component.hpp
    ../../include/ui/output-component.hpp
)


########################################
# EXECUTABLE
########################################

if (ERA_SIM_BUILD_GUI)
  qt5_add_resources(UI_RESOURCES qml.qrc)
  qt5_wrap_cpp(UI_HEADERS_WRAPPED ${UI_HEADERS})

  add_executable(
    era-sim
    ${UI_SOURCES}
    ${UI_RESOURCES}
    ${UI_HEADERS_WRAPPED}
  )
  target_link_libraries(era-sim era-sim-core)

  qt5_use_modules(era-sim Widgets Qml Quick)
endif()
