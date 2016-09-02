/* C++ Assembler Interpreter
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
 * along with this program. If not, see http://www.gnu.org/licenses/.*/

import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4

Item {
    // Displays the registers in a tree-like structure.
    TreeView {
        anchors.fill: parent
        backgroundVisible: false
        style: TreeViewStyle {
            frame: Rectangle {
                border.width: 0
                color: "#00000000"
            }
        }
        alternatingRowColors: false
        headerVisible: false
        selectionMode: SelectionMode.NoSelection

        // Uses the RegisterModel to populate the registers.
        model: registerModel

        itemDelegate: Item {
            // Register Title
            Label {
                id: registerTitleLabel
                anchors.left: registerContentItem.left
                anchors.leftMargin: 1
                anchors.top: parent.top
                text: model.Title
                font.pointSize: 15
                font.weight: Font.Bold
                color: "#585858"
            }
            // The list of available data formats.
            ComboBox {
                id: dataTypeFormatComboBox
                anchors.verticalCenter: registerContentItem.verticalCenter
                anchors.right: parent.right
                width: 18
                height: registerContentItem.height
                model: ListModel {
                    id: dataTypeFormatModel
                    Component.onCompleted: {
                        // Creates a ListModel from the list of data formats provided by the RegisterModel.
                        var list = model.DataFormatsList;
                        list.forEach(function(entry) {
                            append({"text": entry});
                        });
                    }
                }


                style: ComboBoxStyle {
                    // Selection Indicator
                    label: Item {
                        id: comboBoxSelectionIndicator
                        // Triangle Up
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.topMargin: -3
                            text: "\u25b2"
                            font.pointSize: 7
                        }
                        // Triangle Down
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: -3
                            text: "\u25bc"
                            font.pointSize: 7
                        }
                    }
                    // Remove ComboBox-background
                    background: Rectangle {
                        visible: false
                    }
                }
            }
            Loader {
                id: registerContentItem

                anchors.top: registerTitleLabel.bottom
                anchors.topMargin: 3
                anchors.left: parent.left
                anchors.right: dataTypeFormatComboBox.left

                source: "DefaultRegister.qml"
            }
        }

        rowDelegate: Item {
            height: 50
        }

        TableViewColumn {
            title: "Register"
        }
    }
}
