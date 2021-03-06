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
import Theme 1.0

Item {
    anchors.left: parent.left
    anchors.right: parent.right

    property alias text: macroTextEdit.text
    property alias fontPixelSize: macroTextEdit.font.pixelSize
    property alias fontFamily: macroTextEdit.font.family
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
        color: Theme.editor.macro.background
    }

    Rectangle {
        id: macroSidebar
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: lineNumbering.width + 3

        color: Theme.editor.sidebar.background

        Column {
            id: lineNumbering
            anchors.left: parent.left
            y: textRegion.textMargin/2

            Repeater {
                model: textRegion.lineCount
                delegate: Text {
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

        Component.onCompleted: {
            // Add syntax highlighter that highlights the textEdit's text document.
            editor.addSecondarySyntaxHighlighter(macroTextEdit.textDocument);
        }

        Component.onDestruction: {
            // Destroy the corresponding secondary syntax highlighter in EditorComponent to prevent it
            // from accessing an already destroyed text area text document (would cause
            // segmentation fault on program end) and to prevent the number of highlighters
            // from growing monotonically without ever being reduced.
            editor.deleteSecondarySyntaxHighlighter(macroTextEdit.textDocument);
        }
    }

}
