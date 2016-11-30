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

    menuBar: Menubar {
      id: menubar
      main: window
    }
    toolBar: ToolbarMainWindow{
        id: toolbar
        tabView: tabView
    }

    TabView{
        anchors.fill: parent
        id: tabView

        Component.onCompleted: {
            createProject()
        }
    }

    function createProject() {
        tabView.addTab("Create new project...", tabs);
        tabView.currentIndex = tabView.count - 1;
    }

    function closeProject() {
        var currentTabIndex = tabView.currentIndex;
        tabView.removeTab(currentTabIndex);
        ui.removeProject(currentTabIndex);
    }

    /*Component for a project, instantiated by the TabView*/
    Component{
        id: tabs

        Item {
            id: placeholderItem
            anchors.fill: parent
            ProjectCreationScreen {
                anchors.fill: parent
                tab: parent.parent
                onButtonClicked: {
                    enabled = false;
                    visible = false;
                    ui.addProject(placeholderItem, projectComponent,
                      memorySize, architecture, optionName, parser);
                }
            }
        }
    }

    //this component is instantiated by the addProject method
    Component{
        id: projectComponent
        Item {
          anchors.fill: parent
          Splitview{
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
      standardButtons: StandardButton.Cancel;
      property var onAcceptedFunction
      property alias placeholderText: textField.placeholderText

      TextField {
        id: textField

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
