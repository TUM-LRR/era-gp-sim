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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/



import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4

Item {
    property int memory_size: menuBar.memory_value
    onMemory_sizeChanged: {
        if (memory_size < 0) {
            memory_size = 0
        }

        while (libraryModel.count < memory_size) {
            libraryModel.append({
                                    address: "0x" + pad(
                                                 libraryModel.count.toString(
                                                     16).toUpperCase(), 5),
                                    info: "info"
                                })
        }
        while (libraryModel.count > memory_size) {
            libraryModel.remove(libraryModel.count - 1, 1)
        }
    }

    function pad(n, width, z) {
        z = z || '0'
        n = n + ''
        return n.length >= width ? n : new Array(width - n.length + 1).join(
                                       z) + n
    }

    ListModel {
        id: libraryModel
    }

    TableView {
        id: tableView
        // alternatingRowColors: false
        //anchors.fill: parent
        anchors.top: menuBar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        selectionMode: SelectionMode.NoSelection
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

        TableViewColumn {
            role: "address"
            title: "Adresse"
            movable: false
            resizable: false
            width: 70
        }
        TableViewColumn {
            role: "content"
            title: "Inhalt"
            movable: false
            resizable: false
            width: 80
            delegate: editableContent
        }
        TableViewColumn {
            role: "info"
            title: "Info"
            movable: false
            resizable: false
            width: parent.width - 200
        }
        model: libraryModel
    }

    Component {
        id: editableContent
        TextField {
            id: textFieldMemoryValue
            style: TextFieldStyle {
                background: Rectangle {
                    id: styleRec
                    color: "transparent"
                }
            }
            font.bold: true
            inputMask: "\\0\\xHHHH"
            onActiveFocusChanged: {
                cursorPosition = 2
            }
            onHoveredChanged: {
                //styleRec.border.width=2
            }
            onCursorPositionChanged: {
                //jump to TextField in following memory segment
                if(cursorPosition >= inputMask.length - 4)
                    nextItemInFocusChain(true).forceActiveFocus()
                //jump to TextField in preceeding memory segment
                if(cursorPosition <= 1 && selectedText == "")
                    nextItemInFocusChain(false).forceActiveFocus()
            }
            onTextChanged: {
                textFieldMemoryValue.text = textFieldMemoryValue.text.replace(' ','0')
            }
            Keys.onDeletePressed: {

            }

            placeholderText: "0x0000"
            text: "0x0000"
        }
    }

    MemoryComponent_MenuBar{
        id:menuBar
        height: 35
    }


}
