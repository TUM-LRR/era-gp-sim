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
import QtQuick.Controls 1.5
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Editor test")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    toolBar: ToolBar {
        anchors.fill: parent

        ToolButton {
            Layout.alignment: Qt.AlignRight
            text: "Add module"
        }
    }

    TabView {
        anchors.fill: parent
        anchors.centerIn: parent

        Tab {
            title: "Example.asm"
            anchors.fill: parent
            SplitView {
                id: moduleSplitView
                anchors.fill: parent
                orientation: Qt.Horizontal
                property var numberOfViews: 3

                Rectangle {
                    width: moduleSplitView.width / numberOfViews
                    Layout.minimumWidth: 100
                    Layout.maximumWidth: moduleSplitView.width - 50
                    color: "lightblue"
                    Text {
                        text: "View 1"
                        anchors.centerIn: parent
                    }
                }

                Editor {
                    width: moduleSplitView.width / numberOfViews
                    Layout.minimumWidth: 100
                    Layout.maximumWidth: moduleSplitView.width - 50
                    Layout.fillWidth: true
                }

                Component {
                    id: templateModule
                    Editor {
                        width: moduleSplitView.width / numberOfViews
                        Layout.minimumWidth: 100
                        Layout.maximumWidth: moduleSplitView.width - 50
                        Layout.fillWidth: false
                        //color: Qt.rgba(Math.random(), Math.random(), Math.random(), 1)
//                        ComboBox {
//                            id: moduleSwitcher
//                            anchors.left: parent.left
//                            anchors.top: parent.top
//                            anchors.topMargin: 5

//                        }

//                        Text {
//                            text: "Template"
//                            anchors.centerIn: parent
//                        }

                    }
                }

                Rectangle {
                    id: centerItem
                    width: moduleSplitView.width / moduleSplitView.numberOfViews
                    Layout.minimumWidth: 100
                    Layout.maximumWidth: moduleSplitView.width - 50
                    Layout.fillWidth: false;
                    color: "lightgray"
                    Text {
                        text: "View 2"
                        anchors.centerIn: parent
                    }

                    Button {
                        anchors.centerIn: parent
                        text: "Add Split"
                        onClicked: {
                            var newItem = templateModule.createObject();
                            moduleSplitView.addItem(newItem);
                            moduleSplitView.numberOfViews++;
                            centerItem.width = moduleSplitView/moduleSplitView.numberOfViews;
                            //console.log(MyScript.counter);
                        }
                    }
                }
            }
        }

        Tab {
            title: "Example2.asm"
            Rectangle {
                anchors.fill: parent
                anchors.centerIn: parent
                color: "red"
            }
        }
    }


}
