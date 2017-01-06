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

import QtQuick 2.0
import QtQuick.Controls 1.4

Item {

    Rectangle{
        id: middle
        height: 40
        width: 40
        anchors.centerIn: parent
        visible: false
    }

    Button{
        id: up
        width: middle.width
        height: middle.height
        anchors.left: middle.left
        anchors.bottom: middle.top
        Label{
            font.pointSize: 16
            anchors.centerIn: parent
            text: "\u2191"
        }
        onClicked: {
            inputButtonMod.buttonClicked(4);
        }
    }

    Button{
        id: right
        width: middle.width
        height: middle.height
        anchors.left: middle.right
        anchors.bottom: middle.bottom
        Label {
            font.pointSize: 16
            anchors.centerIn: parent
            text: "\u2192"
        }
        onClicked: {
            inputButtonMod.buttonClicked(1);
        }
    }

    Button{
        id: down
        width: middle.width
        height: middle.height
        anchors.left: middle.left
        anchors.top: middle.bottom
        Label {
            font.pointSize: 16
            anchors.centerIn: parent
            text: "\u2193"
        }
        onClicked: {
            inputButtonMod.buttonClicked(2);
        }
    }

    Button{
        id: left
        width: middle.width
        height: middle.height
        anchors.right: middle.left
        anchors.bottom: middle.bottom
        Label {
            font.pointSize: 16
            anchors.centerIn: parent
            text: "\u2190"
        }
        onClicked: {
            inputButtonMod.buttonClicked(3);
        }
    }

    function settingsButtonPressed(){
        settingsWindowIB.show();
    }

    ButtonInputSW{
        id: settingsWindowIB
    }


}
