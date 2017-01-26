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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Item {
    property int mode: 0  //array based
    property int oldMode: 1 //pipelike
    Rectangle {
        color: "black"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        height: 30

        anchors.verticalCenter: parent.verticalCenter

        TextField {
            id: text
            anchors.fill: parent

            readOnly: false
            maximumLength: 2

            menu: null

            textColor: "white"
            font.pointSize: 12
            font.bold: false

            style: TextFieldStyle {
                background: Rectangle {
                color: "black"
                }
            }

            onLengthChanged: {
                if (mode === 1 && length > 0){
                    accepted();
                }
            }

            onAccepted: {
                inputTextModel.newText(text.text);
                text.text = "";
            }


        }

    }

    Component.onCompleted: {
        text.maximumLength = inputTextModel.getMaximumLength();
    }

    //update Maximum Length and mode
    Connections {
        target: inputTextModel
        // Send when maximum Length was changed
        onMaximumLengthChanged: {
            text.maximumLength = inputTextModel.getMaximumLength();
        }
        onModeChanged: {
            mode = inputTextModel.getMode();
            if(oldMode !== mode){
                text.text = "";
            }
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Left || event.key === Qt.Key_Up || event.key === Qt.Key_Right || event.key === Qt.Key_Down) {
            if(mode === 1){//Pipelike
                var inputValue = 0;
                if(event.key === Qt.Key_Right){
                    inputValue = 1;
                } else if (event.key === Qt.Key_Down) {
                    inputValue = 2;
                } else if (event.key === Qt.Key_Left) {
                    inputValue = 3;
                } else {
                    inputValue = 4;
                }

                inputTextModel.newNumber(inputValue);
            }
        }

    }


    function settingsButtonPressed(){
        settingsWindowIT.show();
    }

    TextInputSW {
        id: settingsWindowIT
    }
}
