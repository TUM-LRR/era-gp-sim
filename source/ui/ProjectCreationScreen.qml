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
import "Components/Common"

Item {
    id: root

    signal buttonClicked(int memorySize, var type)

    property var tab;

    property real marginWidth: width*0.1;
    property real marginHeight: height*0.1;

    property var main;

    //create the project
    Button {
        id: button
        anchors {
            top: numericUpDown.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right

            topMargin: marginHeight
            bottomMargin: marginHeight
            leftMargin: marginWidth
            rightMargin: marginWidth
        }

        text: "create Project"
        onClicked: {
            tab.title = textInputName.text
            root.buttonClicked(numericUpDown._value, comboBox.currentText);
        }
    }


    Text {
        id: memorySizeText
        anchors {
            left: numericUpDown.left
            bottom: numericUpDown.top
        }
        text: "Memory size"
    }

    //choose the memory size
    NumericUpDown {
        id: numericUpDown
        anchors {
            top: textInputName.bottom
            left: parent.horizontalCenter
            right: parent.right

            topMargin: marginHeight
            bottomMargin: 0
            leftMargin: marginWidth
            rightMargin: marginWidth
        }
        _value: 1000
    }

    Text {
        id: archText
        anchors {
            left: comboBox.left
            bottom: comboBox.top
        }
        text: "Architecture"
    }

    //choose the architecture
    ComboBox {
        id: comboBox
        anchors {
            top: textInputName.bottom
            left: parent.left
            right: parent.horizontalCenter

            topMargin: marginHeight
            bottomMargin: 0
            leftMargin: marginWidth
            rightMargin: marginWidth
        }
        model: [ "risc-v"]
    }

    TextField {
        id: textInputName
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right

            topMargin: marginHeight
            bottomMargin: marginHeight
            leftMargin: marginWidth
            rightMargin: marginWidth
        }

        placeholderText: "project name"
    }
}
