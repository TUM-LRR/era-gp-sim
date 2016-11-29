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


import QtQuick 2.0
import QtQuick.Controls 1.4

MenuBar {
    id: menubar
    property var main

    function saveAs(filePath) {
      console.log("save path: " + filePath)
      ui.saveTextAs(tabView.currentIndex, filePath);
    }

    function actionSaveAs() {
      main.fileDialog.onAcceptedFunction = saveAs;
      main.fileDialog.selectExisting = false;
      main.fileDialog.open();
    }

    Menu{
        id: fileMenu
        title: "File"
        MenuItem{
            text: "Load Text..."
            function openTextFile(filePath) {
              ui.loadText(main.currentIndex, filePath);
            }
            onTriggered: {
                main.fileDialog.onAcceptedFunction = openTextFile;
                main.fileDialog.selectExisting = true;
                main.fileDialog.open();
            }
        }
        MenuItem{
            text: "New..."
            onTriggered: {
                main.createProject();
            }
        }
        MenuItem{
            text: "Save"
            onTriggered: {
                ui.saveText(main.currentIndex);
            }
        }
        MenuItem{
            id: saveTextAs
            text: "Save as..."
            onTriggered: {
                actionSaveAs();
            }
        }
        MenuItem{
            text: "Save Snapshot"
            function saveSnapshot(name) {
              console.log("save snapshot: " + name)
              ui.saveSnapshot(tabView.currentIndex, name);
            }
            onTriggered: {
                main.textDialog.onAcceptedFunction = saveSnapshot;
                main.textDialog.placeholderText = "name";
                main.textDialog.open();
            }
        }
        MenuItem{
            text: "Import Snapshot"
            function importSnapshot(name) {
              console.log("import snapshot: " + name);
              var success = snapshotComponent.importSnapshot(name);
            }
            onTriggered: {
              main.fileDialog.onAcceptedFunction = importSnapshot;
              main.fileDialog.selectExisting = true;
              main.fileDialog.open();
            }
        }
        MenuItem{
            text: "Close"
            onTriggered: {
                main.closeProject();
            }
        }
    }

    Menu{
        title: "Settings"
        MenuItem{
            text: "Open"
            onTriggered: {
                console.info("Open Settings triggerd")
                ui.settings();
            }
        }
    }

    Menu{
        title: "Help"
        MenuItem{
            text: "Open"
            onTriggered: {
                console.info("Help open triggerd")
                ui.help();
            }
        }
    }
}
