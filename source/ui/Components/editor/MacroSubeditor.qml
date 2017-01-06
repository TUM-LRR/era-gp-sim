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
import QtQuick.Controls 1.5
import "../Common"

Item {
    anchors.left: parent.left
    anchors.right: parent.right

    property alias text: macroTextEdit.text
    property var expandedHeight: 0
    height: 0
    clip: true

    function expand() {
        height = expandedHeight;
    }

    function collapse() {
        height = 0;
    }

    // Background
    Rectangle {
        anchors.fill: parent
        color: "#F5F5F5"
    }

    Rectangle {
        id: macroSidebar
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: lineNumbersBar.width + 3

        color: "#eeeeeb"

        Column {
            id: lineNumbersBar
            anchors.left: parent.left
            y: textArea.textMargin/2

            Repeater {
                model: textArea.lineCount
                delegate: Label {
                    color: "gray"
                    font: macroTextEdit.font
                    text: index+1
                    height: fontMetrics.height
                }
            }
        }
    }

    FontMetrics {
        id: fontMetrics
        font: macroTextEdit.font
    }

    TextEdit {
        id: macroTextEdit
        anchors.left: macroSidebar.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: -2
        anchors.bottom: parent.bottom

        readOnly: true
        textMargin: 2
        color: "#4A4A4A"
    }

}
