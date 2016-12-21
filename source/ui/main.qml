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

ApplicationWindow {
    id: window
    visible: true
    width: Screen.desktopAvailableWidth*0.7
    height: Screen.desktopAvailableHeight*0.8

    property alias menubar: menubar
    property alias toolbar: toolbar

    menuBar: Menubar {
        id: menubar
        main: window
        Component.onCompleted: {
            window.updateMenuState();
        }
    }
    toolBar: ToolbarMainWindow {
        id: toolbar
    }

    TabView {
        anchors.fill: parent
        id: tabView

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
        function isCurrentProjectValid() {
            return getCurrentProjectItem().projectValid;
        }
    }

    // this function should be called when the tab is switched
    function updateMenuState() {
        if (tabView.count === 0 || !tabView.isCurrentProjectValid()) {
            // deactivate project specific functions if there is no valid project at the current index
            toolbar.hideToolbar();
            menubar.setMenuEnabled(false);
        }
        else {
            // enable menus otherwise (could have been disabled before)
            showMenus();
        }
    }

    function showMenus() {
        toolbar.showToolbar();
        menubar.setMenuEnabled(true);
    }

    function createProject() {
        tabView.addTab("Create new project...", tabs);
        tabView.currentIndex = tabView.count - 1;
    }

    function closeProject() {
        var currentTabIndex = tabView.currentIndex;
        var currentProjectId = tabView.getCurrentProjectId();
        var isValid = tabView.isCurrentProjectValid();
        tabView.removeTab(currentTabIndex);
        if(isValid) {
            ui.removeProject(currentProjectId);
        }
        updateMenuState();
        if(tabView.count === 0) {
            // create a new tab if there is no tab anymore, prevents a blank screen.
            createProject();
        }
    }

    /*Component for a project, instantiated by the TabView*/
    Component {
        id: tabs

        Item {
            id: placeholderItem

            // if there is a project for this tab in the backend, this is true
            property bool projectValid: false

            // index of the project in the project vector
            property int projectId: -1

            // indicates the execution state of this project
            property bool isRunning: false

            anchors.fill: parent
            ProjectCreationScreen {
                anchors.fill: parent
                tab: parent.parent
                onButtonClicked: {
                    enabled = false;
                    visible = false;
                    placeholderItem.projectId = ui.addProject(
                        placeholderItem,
                        projectComponent,
                        memorySize,
                        architecture,
                        optionName,
                        parser);
                    window.showMenus();
                    projectValid = true;
                    }
                }
            }
        }

        //this component is instantiated by the addProject method
        Component {
            id: projectComponent
            Item {
                anchors.fill: parent
                Splitview {
                    anchors.fill: parent

                    SystemPalette {
                        id: systemPalette
                    }

                    handleDelegate: Rectangle {
                        width: 2
                        height: 2
                        color: Qt.darker(systemPalette.window, 1.5)
                    }
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
                        // reparse in case a parse was blocked during execution.
                        editor.parse();
                        placeholderItem.isRunning = false;
                    }
                }
            }
        }

        Connections {
            target: snapshotComponent
            onSnapshotError: {
                errorDialog.text = errorMessage;
                errorDialog.open();
            }
        }

        property alias errorDialog: errorDialog
        property alias fileDialog: fileDialog
        property alias textDialog: textDialog

        //Dialog to show errors
        MessageDialog {
            id: errorDialog
            title: "error"
            standardButtons: StandardButton.Ok
            onAccepted: {
                close();
            }
        }

        //File dialog for selecting a file
        FileDialog {
            id: fileDialog
            property var onAcceptedFunction
            selectExisting: false
            selectFolder: false
            selectMultiple: false
            onAccepted: {
                onAcceptedFunction(fileDialog.fileUrl);
            }
        }

        // Dialog to input text
        Dialog {
            id: textDialog
            title: "Save snapshot"
            standardButtons: StandardButton.Cancel;
            property var onAcceptedFunction
            property alias placeholderText: textField.placeholderText
            Text {
                id: description
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                wrapMode: Text.WordWrap
                textFormat: Text.StyledText
                text: "<p>Save a snapshot of the current register and memory state to disk. " +
                "Your snapshot files can be found here:</p> " +
                "<a href=\"" + snapshotComponent.getSnapshotBasePath().toString() + "\">" +
                snapshotComponent.getSnapshotBasePath().toString() + "</a>"
                onLinkActivated: Qt.openUrlExternally(snapshotComponent.getSnapshotBasePath())
            }
            TextField {
                id: textField
                anchors.topMargin: 10
                anchors.top: description.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                onTextChanged: {
                    if(text == "") {
                        textDialog.standardButtons = StandardButton.Cancel;
                    }
                    else {
                        textDialog.standardButtons = StandardButton.Cancel | StandardButton.Save;
                    }
                }
            }
            onAccepted: {
                onAcceptedFunction(textField.text);
                textField.text = "";
            }
        }
    }
