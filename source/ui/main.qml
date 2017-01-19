
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
import QtQuick.Dialogs 1.2

import "Components"
import "Components/Menubar"
import "Components/Toolbar"
import "Components/ProjectCreation"
import "Components/Settings"
import "Components/Snapshots"
import Theme 1.0

ApplicationWindow {
  id: window
  visible: true
  width: Theme.window.initialWidth
  height: Theme.window.initialHeight
  color: Theme.window.background

  property alias menubar: menubar
  property alias toolbar: toolbar
  property alias config: config

  menuBar: Menubar {
    id: menubar
    main: window
    Component.onCompleted: {
      window.updateMenuState();
    }
  }
  toolBar: ToolbarComponent {
    id: toolbar
  }

  SettingsWindow { id: config }

  TabView {
    id: tabView
    anchors.fill: parent

    Component.onCompleted: {
      createProject()
    }

    onCurrentIndexChanged: {
      updateMenuState();
    }

    // get the project of the current tab, is undefined if there is no tab.
    function getCurrentProjectItem() {
      return tabView.getTab(tabView.currentIndex).item;
    }

    // get the id of the project in the current tab, undefined if there is no tab.
    function getCurrentProjectId() {
      return getCurrentProjectItem().projectId;
    }

    // returns false if there is only a creation screen in the current tab.
    // Undefined if there is no tab.
    function currentProjectIsReady() {
      return getCurrentProjectItem().projectValid;
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
    toolbar.running = tabView.getCurrentProjectItem().running;
    menubar.setMenuEnabled(true);
  }

  function expand() {
    window.width = Theme.window.width * Screen.desktopAvailableWidth;
    window.height = Theme.window.height * Screen.desktopAvailableHeight;
    window.x = (Screen.width - window.width) / 2
    window.y = (Screen.height - window.height) / 2
  }

  function createProject() {
    tabView.addTab("", tabs);
    tabView.currentIndex = tabView.count - 1;
  }

  function closeProject() {
    var currentTabIndex = tabView.currentIndex;
    var currentProjectId = tabView.getCurrentProjectId();
    var isReady = tabView.currentProjectIsReady();

    tabView.removeTab(currentTabIndex);

    if(!isReady) window.close();

    ui.removeProject(currentProjectId);
    updateMenuState();

    // Create a new tab if there is no tab
    // anymore, to prevent a blank screen.
    if(tabView.count === 0) {
      createProject();
    }
  }

  // Component for a project, instantiated by the TabView
  Component {
    id: tabs

    Item {
      id: placeholderItem

      // If there is a project for this tab in the backend, this is true.
      property bool projectValid: false

      // Index of the project in the project vector.
      property int projectId: -1

      // Indicates the execution state of this project.
      property bool running

      anchors.fill: parent
      ProjectCreationScreen {
        anchors.fill: parent
        onCreateProject: {
          enabled = false;
          visible = false;

          parent.parent.title = projectName;
          placeholderItem.projectId = ui.addProject(placeholderItem,
            projectComponent,
            memorySize,
            architecture,
            optionName,
            parser
          );
          window.expand();
          window.showMenus();
          projectValid = true;
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

        SystemPalette {
          id: systemPalette
        }

        Connections {
          target: guiProject
          onSaveTextAs: {
            menubar.actionSaveAs();
          }
          onError: {
            window.errorDialog.text = errorMessage;
            window.errorDialog.open();
          }
          onExecutionStopped: {
            toolbar.rowLayout.setStoppedState();
            editor.parse();
            placeholderItem.isRunning = false;
          }
        }
      }

      Connections {
        target: guiProject
        onSaveTextAs: menubar.actionSaveAs();
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
      }
    }
  }

  property alias errorDialog: errorDialog
  property alias fileDialog: fileDialog
  property var snapshotDialog: SnapshotDialog { }

  ErrorDialog { id: errorDialog }

  FileDialog {
    id: fileDialog
    property var onAcceptedFunction
    selectExisting: false
    selectFolder: false
    selectMultiple: false
    onAccepted: {
      ui.saveTextAs(tabView.getCurrentProjectId(), fileDialog.fileUrl);
    }
  }
}
