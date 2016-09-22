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
import QtQuick.Layouts 1.3
import "../Common"

Item {
    property alias memory_value: memory_size_chooser._value

    width: parent.width

    //background
    Rectangle{
        anchors.fill:parent
        gradient: Gradient {
                    GradientStop { position: 0.0;  color: "#f2f5f6" }
                    GradientStop { position: 0.37; color: "#e3eaed" }
                    GradientStop { position: 1.0;  color: "#c8d7dc" }
                }
    }

    RowLayout {
        id: menuBar

        //styling
        spacing: 5
        width: parent.width
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.topMargin: 2
        anchors.bottomMargin: 2

        //children
        ComboBox {
            id: numericRepresentationChooser
            property alias items: model
            //content
            Layout.alignment: Qt.AlignLeft
            currentIndex: 2

            model: ListModel {
                    id: model
                    ListElement { text: "Binary"; shortform:"BIN"; mask: "xxxxx" }
                    ListElement { text: "Octal"; shortform:"OCT"; mask: "xxxxx" }
                    ListElement { text: "Hexadecimal"; shortform:"HEX"; mask: "xxxx" }
                }
            //functions
            onCurrentIndexChanged:{
                console.log("representation of numbers changed")
            }

            /*ToolTip_new {
            id: tooltip1
            //width: 200
            //text: "Enter the text here. asdfljas la sjdlöaöls djfaösd jlasdflaksjd fas "
            }*/
        }
        NumericUpDown {
            id: memory_size_chooser
            anchors.left: parent.left
            anchors.leftMargin: 150
        }
        Label {
            id: memory_size
            anchors.right: parent.right
            anchors.rightMargin: 20
            text: memory_size_chooser._value.toString() +" / "+ numericRepresentationChooser.items.get(numericRepresentationChooser.currentIndex).shortform


        }
    }
}
