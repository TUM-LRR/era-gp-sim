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
    //property int number_bits: numericRepresentationChooser.items.get(numericRepresentationChooser.currentIndex).bits;
    property int number_bits: 8
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

        headerVisible: false

        TableViewColumn {
            role: "address" + number_bits
            title: "Adresse"
            movable: false
            resizable: true
            width: 70
        }
        TableViewColumn {
            role: "info"
            title: "Info"
            movable: false
            resizable: true
            width: 100 //parent.width - ((tableView.columnCount - 2) * 80) - (tableView.getColumn(0).width) - (25)
        }
        model: memoryModel

        Component.onCompleted: {
            tableView.insertColumn( tableView.columnCount - 1, column);
        }
    }

    Component {
        id: column
        TableViewColumn {
            role: "bin" + number_bits
            title: "Inhalt"
            movable: false
            resizable: true
            width: 80
            delegate: inputBox
        }
    }

    Component {
        id: inputBox
        TextField {
            id: textFieldMemoryValue
            text: styleData.value

            onEditingFinished: {
                    memoryModel.setValue(styleData.row, textFieldMemoryValue.text, number_bits, tableView.getColumn(styleData.column).role);
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
            //inputMask: "\\0\\xHH"
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
                    memoryModel.setValue(styleData.row, textFieldMemoryValue.text, tableView.getColumn(styleData.column).role, number_bits);
            }

            placeholderText: "0x00"
            text: model.value
        }
    }


    Rectangle {
        id: menuBar
        height: 25
        width: parent.width

        ListView {
            id: header
            height: parent.height
            width:  parent.width - 25
            //move header according to tableView
            x: -tableView.flickableItem.contentX

            orientation: Qt.Horizontal

           Connections {
                target: tableView
                onColumnCountChanged: {
                    while(headerDropdownList.count < tableView.columnCount - 1)
                        headerDropdownList.append(ListElement);
                    while(headerDropdownList.count > tableView.columnCount - 1)
                        headerDropdownList.remove(headerDropdownList.count - 1);
                }
            }


            model: ListModel {
                id: headerDropdownList
            }
            delegate: Rectangle {
                width: bitChooser.width + resizer.width
                height: 25

                    ComboBox {
                    id: bitChooser
                    height: 25
                    width: resizer.x - bitChooser.x

                    onWidthChanged: {
                        tableView.getColumn(index).width = bitChooser.width + resizer.width;
                    }

                    model: (tableView.getColumn(index).role === "address" + number_bits)? modelBits : modelNumeric;

                    ListModel {
                        id: modelBits
                        ListElement { text: "8 Bit"; bits: 8 }
                        ListElement { text: "16 Bit"; bits: 16 }
                        ListElement { text: "32 Bit"; bits: 32 }
                    }
                    ListModel {
                        id: modelNumeric
                        ListElement { text: "Binary"; role: "bin" }
                        ListElement { text: "Octal"; role: "oct" }
                        ListElement { text: "Hexadecimal"; role: "hex" }
                        ListElement { text: "Decimal"; role: "dec" }
                        ListElement { text: "Decimal (signed)"; role: "decs" }
                    }

                    onCurrentIndexChanged: {
                        if(model === modelBits) {
                            number_bits = model.get(bitChooser.currentIndex).bits;
                        }
                        else {
                            // explicitly create a property binding for number_bits so the role gets updated correctly
                            tableView.getColumn(index).role = Qt.binding(function() {
                                return model.get(bitChooser.currentIndex).role + number_bits })
                        }
                    }
                }
                Rectangle {
                    id: resizer
                    height: 25
                    width: 5
                    x: bitChooser.x + 50
                    MouseArea {
                        drag.axis: Drag.XAxis
                        drag.target: resizer
                        anchors.fill: parent
                        cursorShape: Qt.SizeHorCursor
                    }
                }
            }
        }


        Button {
            anchors.right: parent.right
            width: 25
            height: 25
            text: "+"

            onClicked: {
                tableView.insertColumn( tableView.columnCount - 1, column);
                //tableView.horizontalScrollBarPolicy = Qt.ScrollBarAlwaysOff;
            }
        }
    }
}
