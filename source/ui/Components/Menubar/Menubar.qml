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
import QtQuick.Controls 1.4

MenuBar {
  id: menubar
  property var main

  signal saveSnapshotRequest(int sourceProjectId);
  signal saveSnapshotAsRequest(int sourceProjectId);

  Menu {
    id: editorMenu
    title: "Editor"

    MenuItem {
      id: settings
      text: "Settings"
      shortcut: "Ctrl+,"

      onTriggered: main.settings.show();
    }

    MenuItem {
      id: quit
      text: "Quit Application"
      shortcut: "Ctrl+Shift+W"
      onTriggered: window.close()
    }

    MenuItem {
      id: openFileOption
      text: "Open File"
      shortcut: "Alt+O"
      onTriggered: {
        main.fileDialog.onAcceptedFunction = function(filePath) {
          ui.loadText(tabView.currentProjectId(), filePath);
        };
        main.fileDialog.selectExisting = true;
        main.fileDialog.open();
      }
    }

    MenuItem {
      id: saveFileOption
      text: "Save File"
      shortcut: "Alt+S"
      onTriggered: ui.saveText(tabView.currentProjectId())
    }

    MenuItem {
      id: saveFileAsOption
      text: "Save File As"
      shortcut: "Alt+Shift+S"
      onTriggered: actionSaveAs();
    }
  }

  Menu {
    id: projectMenu
    title: "Project"

    MenuItem {
      text: "New Project"
      shortcut: "Ctrl+N"
      onTriggered: main.createProject()
    }

    MenuItem {
      text: "Close Project"
      shortcut: "Ctrl+W"
      onTriggered: main.closeCurrentProject()
    }

    MenuItem {
      id: openProject
      text: "Open Project"
      shortcut: "Ctrl+Alt+O"
      onTriggered: {
        main.fileDialog.onAcceptedFunction = function(path) {
          main.loadProject(path);
        };
        main.fileDialog.selectExisting = true;
        main.fileDialog.open();
      }
    }

    MenuItem {
      id: saveProject
      text: "Save Project"
      shortcut: "Ctrl+Alt+S"
      onTriggered: {
        // Call store settings method here directly, as a signal could be delayed.
        tabView.currentProjectItem().storeProjectSettings();
        ui.saveProject(tabView.currentProjectId());
      }
    }

    MenuItem {
      id: saveProjectAs
      text: "Save Project As"
      shortcut: "Ctrl+Shift+Alt+S"
      onTriggered: actionSaveProjectAs();
    }

    MenuItem {
      id: openSnapshot
      text: "Open Snapshot"
      shortcut: "Ctrl+O"
      onTriggered: {
        main.fileDialog.onAcceptedFunction = function(name) {
          snapshotComponent.importSnapshot(name);
        };
        main.fileDialog.selectExisting = true;
        main.fileDialog.open();
      }
    }

    MenuItem {
      id: saveSnapshot
      text: "Save Snapshot"
      shortcut: "Ctrl+S"
      onTriggered: saveSnapshotRequest(tabView.currentProjectId())
    }

    MenuItem {
      id: saveSnapshotAs
      text: "Save Snapshot As"
      shortcut: "Ctrl+Shift+S"
      onTriggered: saveSnapshotAsRequest(tabView.currentProjectId())
    }

    function enable(yes) {
      saveProject.enabled = yes;
      saveProjectAs.enabled = yes;
      openSnapshot.enabled = yes;
      saveSnapshot.enabled = yes;
    }
  }

  function saveAs(filePath) {
    ui.saveTextAs(tabView.currentProjectId(), filePath);
  }

  function actionSaveAs() {
    main.fileDialog.onAcceptedFunction = function(filePath) {
      ui.saveTextAs(tabView.currentProjectId(), filePath);
    };

    main.fileDialog.selectExisting = false;
    main.fileDialog.open();
  }

  function actionSaveProjectAs() {
    main.fileDialog.onAcceptedFunction = function(path) {
      // Call store settings method here directly, as a signal could be delayed.
      tabView.currentProjectItem().storeProjectSettings();
      ui.saveProjectAs(tabView.currentProjectId(), path);
    };

    main.fileDialog.selectExisting = false;
    main.fileDialog.open();
  }

  function setMenuEnabled(yes) {
    openFileOption.enabled = yes;
    saveFileOption.enabled = yes;
    saveFileAsOption.enabled = yes;
    projectMenu.enable(yes);
  }
}
