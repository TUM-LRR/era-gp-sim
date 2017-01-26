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
  input-button-model.cpp
  input-text-model.cpp
  input-click-model.cpp
  translateable-processing.cpp
  theme.cpp
  settings.cpp
)

set(UI_HEADERS
  ${CMAKE_SOURCE_DIR}/include/ui/register-model.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/gui-project.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/syntaxhighlighter.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/editor-component.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/memory-component-presenter.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/ui.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/snapshot-component.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/output-component.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/input-button-model.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/input-click-model.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/input-text-model.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/theme.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/settings.hpp
)


########################################
# EXECUTABLE
########################################

if (ERASIM_BUILD_GUI)
  qt5_add_resources(UI_RESOURCES qml.qrc)
  qt5_wrap_cpp(UI_HEADERS_WRAPPED ${UI_HEADERS})

  add_executable(
    erasim
    ${UI_SOURCES}
    ${UI_RESOURCES}
    ${UI_HEADERS_WRAPPED}
  )
  target_link_libraries(erasim erasim-core)

  qt5_use_modules(erasim Widgets Qml Quick)
endif()
