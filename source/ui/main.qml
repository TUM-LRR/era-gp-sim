
/*
* C++ Assembler Interpreter
* Copyright (C) 2016 Chair of Computer Architecture
* at Technical University of Munich
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

import "Components"
import "Components/Menubar"
import "Components/Toolbar"
import "Components/ProjectCreation"
import "Components/Settings"
import "Components/tabview"
import "Components/Snapshots"
import "Fonts/"

import Theme 1.0

ApplicationWindow {
  id: window
  visible: true
  width: Theme.window.initialWidth
  height: Theme.window.initialHeight
  style: ApplicationWindowStyle {
    background: Rectangle { color: Theme.window.background }
  }

  property alias menubar: menubar
  property alias toolbar: toolbar
  property alias settings: settings

  menuBar: Menubar {
    id: menubar
    main: window
    Component.onCompleted: window.updateMenuState();
  }

  toolBar: ToolbarComponent { id: toolbar }
  SettingsWindow { id: settings }

  ProjectsTabView {
    id: tabView
    anchors.fill: parent

    Connections {
      target: ui
      onProjectCreationFailed: {
        tabView.removeTab(index);
        main.errorDialog.text = errorMessage;
        main.errorDialog.open();
      }
    }
  }

  // This function should be called when the tab is switched
  function updateMenuState() {
    if (tabView.count === 0 || !tabView.currentProjectIsReady()) {
      // Deactivate project specific functions if there is no
      // valid project at the current index.
      toolbar.enabled = false;
      menubar.setMenuEnabled(false);
    } else {
      // Enable menus otherwise (could have been disabled before).
      showMenus();
    }
  }

  function showMenus() {
    toolbar.enabled = true;
    toolbar.running = tabView.currentProjectItem().running;
    menubar.setMenuEnabled(true);
  }

  function expand() {
    window.width = Theme.window.width * Screen.desktopAvailableWidth;
    window.height = Theme.window.height * Screen.desktopAvailableHeight;
    window.x = (Screen.width - window.width) / 2
    window.y = (Screen.height - window.height) / 2
  }

  function loadProject(path) {
    var tab;
    // create a new tab if there is already a real project in the current tab.
    if (!tabView.currentProjectIsReady()) {
      tab = tabView.currentProjectItem();
    } else {
      tab = tabView.addTab("", tabComponent);
      tabView.currentIndex = tabView.count - 1;
      tab = tab.item;
    }
    tab.setCreationScreenInvisible();
    tab.projectId =
        ui.loadProject(tab, projectComponent, path, tabView.currentIndex);
    tab.tabReady();
  }

  function createProject() {
    tabView.addTab("", tabComponent);
    tabView.currentIndex = tabView.count - 1;
  }

  function closeCurrentProject() {
    closeProject(tabView.currentIndex);
  }

  function closeProject(projectIndex) {
    var projectId = tabView.projectId(projectIndex);
    var isReady = tabView.projectIsReady(projectIndex);
    if (tabView.count === 1 && !isReady) {
      window.close();
    } else {
      tabView.removeTab(projectIndex);
      ui.removeProject(projectId);
      updateMenuState();
    }
  }

  // Component for a project, instantiated by the TabView
  Component {
    id: tabComponent
    Item {
      id: placeholderItem

      // If there is a project for this tab in the backend, this is true.
      property bool projectValid: false

      // Index of the project in the project vector.
      property int projectId: -1

      // Indicates the execution state of this project.
      property bool running

      anchors.fill: parent

      // Disable the project creation screen.
      function setCreationScreenInvisible() {
        creationScreen.visible = false;
        creationScreen.enabled = false;
      }

      // Complete the setup of the tab.
      function tabReady() {
        window.expand();
        window.showMenus();
        projectValid = true;
      }

      ProjectCreationScreen {
        id: creationScreen
        anchors.fill: parent
        onCreateProject: {
          setCreationScreenInvisible();

          parent.parent.title = projectName;
          placeholderItem.projectId = ui.addProject(placeholderItem,
            projectComponent,
            memorySize,
            architecture,
            optionName,
            parser,
            projectName
          );

          tabReady();
        }
      }
    }
  }

  // This component is instantiated by the addProject method
  Component {
    id: projectComponent
    Item {
      anchors.fill: parent
      Splitview {
        anchors.fill: parent

        SystemPalette { id: systemPalette }

        // height and witdth have to be greater than 1 to achieve correct behaviour on
        // devices with touchscreen
        handleDelegate: Rectangle {
          width: Theme.splitview.handleWidth
          height: Theme.splitview.handleHeight
          color: Theme.splitview.handleColor
        }
      }

      Connections {
        target: guiProject
        onSaveTextAs: menubar.actionSaveAs();
        onSaveProjectAsSignal: menubar.actionSaveProjectAs();
        onError: {
          window.errorDialog.text = errorMessage;
          window.errorDialog.open();
        }
        onExecutionStopped: {
          toolbar.running = false;
          placeholderItem.running = false;

          // Reparse in case a parse was blocked during execution.
          editor.parse();
        }
        onProjectNameChanged: placeholderItem.parent.title = name;
      }
    }
  }

  property alias errorDialog: errorDialog
  property alias fileDialog: fileDialog

  ErrorDialog { id: errorDialog }

  FileDialog {
    id: fileDialog
    property var onAcceptedFunction
    selectExisting: false
    selectFolder: false
    selectMultiple: false
    onAccepted: onAcceptedFunction(fileDialog.fileUrl);
  }

  FontLoaderComponent {}
}
