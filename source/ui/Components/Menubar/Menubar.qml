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

  function saveAs(filePath) {
    ui.saveTextAs(tabView.getCurrentProjectId(), filePath);
  }

  function actionSaveAs() {
    main.fileDialog.onAcceptedFunction = saveAs;
    main.fileDialog.selectExisting = false;
    main.fileDialog.open();
  }

  function setMenuEnabled(yes) {
    openFileOption.enabled = yes;
    saveFileOption.enabled = yes;
    saveFileAsOption.enabled = yes;
    projectMenu.enable(yes);
  }

  Menu {
    id: editorMenu
    title: "Editor"

    MenuItem {
      id: settings
      text: "Settings"
      shortcut: "Ctrl+,"

      onTriggered: main.config.show();
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
      function openTextFile(filePath) {
        ui.loadText(tabView.getCurrentProjectId(), filePath);
      }
      onTriggered: {
        main.fileDialog.onAcceptedFunction = openTextFile;
        main.fileDialog.selectExisting = true;
        main.fileDialog.open();
      }
    }

    MenuItem {
      id: saveFileOption
      text: "Save File"
      shortcut: "Alt+S"
      onTriggered: {
        ui.saveText(tabView.getCurrentProjectId());
      }
    }

    MenuItem {
      id: saveFileAsOption
      text: "Save File As"
      shortcut: "Alt+Shift+S"
      onTriggered: actionSaveAs
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
      onTriggered: {
        main.closeProject();
      }
    }

    MenuItem {
      id: openSnapshot
      text: "Open Snapshot"
      shortcut: "Ctrl+O"
      function importSnapshot(name) {
        var success = snapshotComponent.importSnapshot(name);
      }
      onTriggered: {
        main.fileDialog.onAcceptedFunction = importSnapshot;
        main.fileDialog.selectExisting = true;
        main.fileDialog.open();
      }
    }

    MenuItem {
      id: saveSnapshot
      text: "Save Snapshot"
      shortcut: "Ctrl+S"
      onTriggered: main.snapshotDialog.open()
    }

    function enable(yes) {
      openSnapshot.enabled = yes;
      saveSnapshot.enabled = yes;
    }
  }
}
