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
        alternatingRowColors: false
        headerVisible: false
        selectionMode: SelectionMode.NoSelection

        // Uses the RegisterModel to populate the registers.
        model: registerModel

        itemDelegate: Item {
            // Register Title
            Label {
                id: registerTitleLabel
                anchors.left: registerTextField.left
                anchors.leftMargin: 1
                anchors.top: parent.top
                text: model.RegisterTitle
                font.pointSize: 15
                font.weight: Font.Bold
                color: "#585858"
            }
            // The list of available data formats.
            ComboBox {
                id: dataTypeFormat
                anchors.verticalCenter: registerTextField.verticalCenter
                anchors.right: parent.right
                width: 70
                height: 1.2*registerTextField.height
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
            }
            // The register content.
            TextField {
                id: registerTextField
                anchors.top: registerTitleLabel.bottom
                anchors.topMargin: 3
                anchors.left: parent.left
                anchors.right: dataTypeFormat.left

                text: model.RegisterContent
                inputMask: model.RegisterDisplayFormatString
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
