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
    property alias fileMenu: fileMenu

    function saveSnapshot(name) {
        ui.saveSnapshot(tabView.getCurrentProjectIndex(), name);
    }

    function actionSnapshot() {
        main.textDialog.onAcceptedFunction = saveSnapshot;
        main.textDialog.placeholderText = "name";
        main.textDialog.open();
    }

    function saveAs(filePath) {
        console.log("save path: " + filePath)
        ui.saveTextAs(tabView.getCurrentProjectIndex(), filePath);
    }

    function actionSaveAs() {
        main.fileDialog.onAcceptedFunction = saveAs;
        main.fileDialog.selectExisting = false;
        main.fileDialog.open();
    }

    Menu {
        id: fileMenu
        title: "File"

        // sets the enabled values of the project related menu entries.
        function setProjectMenuEnabled(value) {
            loadCode.enabled = value;
            saveCode.enabled = value;
            saveCodeAs.enabled = value;
            importSnapshot.enabled = value;
            saveSnapshot.enabled = value;
        }

        MenuItem {
            text: "New Project..."
            onTriggered: {
                main.createProject();
            }
        }

        MenuItem {
            text: "Close Project"
            onTriggered: {
                main.closeProject();
            }
        }

        MenuSeparator {}

        MenuItem {
            id: loadCode
            text: "Load Code..."
            function openTextFile(filePath) {
                ui.loadText(tabView.getCurrentProjectIndex(), filePath);
            }
            onTriggered: {
                main.fileDialog.onAcceptedFunction = openTextFile;
                main.fileDialog.selectExisting = true;
                main.fileDialog.open();
            }
        }

        MenuItem {
            id: saveCode
            text: "Save Code"
            onTriggered: {
                ui.saveText(tabView.getCurrentProjectIndex());
            }
        }

        MenuItem {
            id: saveCodeAs
            text: "Save Code as..."
            onTriggered: {
                actionSaveAs();
            }
        }

        MenuItem {
            id: importSnapshot
            text: "Import Snapshot"
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
            onTriggered: {
                actionSnapshot();
            }
        }
    }
}
