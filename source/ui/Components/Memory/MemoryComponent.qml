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
    //property int number_bytes: numericRepresentationChooser.items.get(numericRepresentationChooser.currentIndex).bits;
    property int number_bytes: 8
    property alias tableView: tableView

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

        headerDelegate: headerDelegate

        TableViewColumn {
            role: "address" + number_bytes
            title: "Adresse"
            movable: false
            resizable: false
            width: 70
        }
        TableViewColumn {
            role: "value"
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
        model: memoryModel
    }

    Component {
        id: headerDelegate

        ComboBox {
            id: bitChooser
            height: 25

            // add model according to column
            model: (styleData.value === "Adresse")? modelBits : modelNumeric;

            ListModel {
                id: modelBits
                ListElement { text: "8 Bit"; bits: 8 }
                ListElement { text: "16 Bit"; bits: 16 }
                ListElement { text: "32 Bit"; bits: 32 }
            }
            ListModel {
                id: modelNumeric
                ListElement { text: "Binary"; bits: 8 }
                ListElement { text: "Octal"; bits: 16 }
                ListElement { text: "Hexadecimal"; bits: 32 }
                ListElement { text: "Decimal"; bits: 32 }
                ListElement { text: "Decimal (signed)"; bits: 32 }
            }
            Connections {
                target: styleData
                onPressedChanged: {
                    if(styleData.pressed)
                        bitChooser.__popup.toggleShow();
                }
            }
            onCurrentIndexChanged: {
                if(model === modelBits)
                    number_bytes = model.get(bitChooser.currentIndex).bits;
                else
                    tableView.getColumn(styleData.column).role = "value" + model.get(bitChooser.currentIndex).bits;

            }
        }
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
            inputMask: "\\0\\xHH"
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

            Keys.onDeletePressed: {

            }
            onEditingFinished: {
                    memoryModel.setValue(styleData.row, textFieldMemoryValue.text);
                testvalue=Math.random(100);
                console.log(testvalue);
            }

            placeholderText: "0x00"
            text: model.value

        }
    }



    ListView {
        id: menuBar
        height: 25
        width: parent.width
       Connections {
            target: tableView
            onColumnCountChanged: {
                while(headerDropdownList.count < tableView.columnCount)
                    headerDropdownList.append(ListElement);
                while(headerDropdownList.count > tableView.columnCount)
                    headerDropdownList.remove(headerDropdownList.count - 1);
                console.log("columncount changed "+headerDropdownList.count);
            }
        }
        orientation: Qt.Horizontal

        model: ListModel {
            id: headerDropdownList
        }
        delegate: ComboBox {
            id: bitChooser
            height: 25
            width: tableView.getColumn(index).width

            model: (tableView.getColumn(index).role === "address" + number_bytes)? modelBits : modelNumeric;

            ListModel {
                id: modelBits
                ListElement { text: "8 Bit"; bits: 8 }
                ListElement { text: "16 Bit"; bits: 16 }
                ListElement { text: "32 Bit"; bits: 32 }
            }
            ListModel {
                id: modelNumeric
                ListElement { text: "Binary"; bits: 8 }
                ListElement { text: "Octal"; bits: 16 }
                ListElement { text: "Hexadecimal"; bits: 32 }
                ListElement { text: "Decimal"; bits: 32 }
                ListElement { text: "Decimal (signed)"; bits: 32 }
            }

            onCurrentIndexChanged: {
                if(model === modelBits)
                    number_bytes = model.get(bitChooser.currentIndex).bits;
                else
                    tableView.getColumn(index).role = "value" + model.get(bitChooser.currentIndex).bits;
            }
        }
    }
}
