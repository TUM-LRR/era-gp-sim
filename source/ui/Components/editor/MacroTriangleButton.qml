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
import QtQuick.Controls.Styles 1.4

Button {
    id: expandButton

    // Saves the macro's index this button represents. Used for specifying which macro has to be expanded when this button
    // is clicked.
    property var macroIndex: undefined
    property var onExpandedChanged: function (currentMacroIndex) {}

    text: "\u25b6"
    style: ButtonStyle {
        label: Text {
            text: control.text
            font.pointSize: 10
            color: "gray"
        }
        background: Rectangle {
            visible: false
        }
    }
    onClicked: {
        text = (text == "\u25b6") ? "\u25bc" : "\u25b6";
        onExpandedChanged(macroIndex);
    }

    function setExpanded() {
        text = "\u25bc";
    }

    function setCollapsed() {
        text = "\u25b6";
    }
}
