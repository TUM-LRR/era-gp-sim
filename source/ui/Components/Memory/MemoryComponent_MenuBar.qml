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

    width: parent.width

    //background
    Rectangle{
        anchors.fill:parent
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
                    ListElement { text: "8 Bit"; bytes: 1 }
                    ListElement { text: "16 Bit"; bytes: 2 }
                    ListElement { text: "32 Bit"; bytes: 3 }
                }
            //functions
            onCurrentIndexChanged:{
                console.log("representation of numbers changed")
            }
        }
    }
}
