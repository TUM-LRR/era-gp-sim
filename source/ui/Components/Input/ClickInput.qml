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



Item {
    property int inputItemIndex
    anchors.top: parent.top

    Rectangle{
        id: area
        width: 255
        height: 255
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: xRect.bottom
        //anchors.verticalCenter:  parent.verticalCenter

        color: "blue"

        MouseArea{
            anchors.fill: parent
            onClicked: {
                //console.info(mouse.x);
                //console.info(mouse.y);
                inputClickMod.newClick(mouse.x, mouse.y);
            }
        }

    }
    Rectangle{
        id: xRect
        height: 40
        width: area.width
        anchors.left: area.left
        anchors.top: parent.top
        anchors.topMargin: 10
        color: "transparent"

        Text{
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            text: "0"
        }

        Text {
            font.pointSize: 12
            text: "x\n\u2192" ;
            anchors.centerIn: parent
        }

        Text{
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            text: "255"
        }
    }

    Rectangle{
        width: 40
        height: area.height
        anchors.right: area.left
        anchors.top: area.top
        color: "transparent"

        Text{
            anchors.top: parent.top
            anchors.right: parent.right
            text: "0  "
        }

        Text {
            font.pointSize: 12
            anchors.centerIn: parent
            text: "y \u2193"
        }

        Text{
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            text: "255  "
        }
    }

    function settingsButtonPressed(){
        settingsWindowIC.show();
    }

    ClickInputSW{
        id: settingsWindowIC
    }

}
