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
  input-key-model.cpp
  console-component.cpp
  pixel-display-color-mode.cpp
  pixel-display-options.cpp
  pixel-display-painted-item.cpp
  input-click-model.cpp
  translateable-processing.cpp
  theme.cpp
  settings.cpp
  project-settings.cpp
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
  ${CMAKE_SOURCE_DIR}/include/ui/input-key-model.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/pixel-display-color-mode.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/pixel-display-options.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/pixel-display-painted-item.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/input-click-model.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/console-component.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/theme.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/settings.hpp
  ${CMAKE_SOURCE_DIR}/include/ui/project-settings.hpp
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
	# The icon resource for windows
	${WIN_RES_FILE}
  )

  target_link_libraries(era-sim era-sim-core)
  target_link_libraries(era-sim Qt5::Widgets Qt5::Qml Qt5::Quick Qt5::Svg)
endif ()
