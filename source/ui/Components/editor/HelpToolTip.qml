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
import "../Common"

Rectangle {
    id: helpToolTip

    property alias helpText: helpOverlay.text
    property alias maxWidth: helpOverlay.maxWidth
    property alias maxHeight: helpOverlay.maxHeight
    property alias relativeX: helpOverlay.relativeX
    property alias relativeY: helpOverlay.relativeY
    property alias realWidth: helpOverlay.realWidth
    property alias realHeight: helpOverlay.realHeight

    radius: 3
    color: Qt.rgba(32.0/255.0, 57.0/255.0, 77.0/255.0, 0.85)
    border.color: Qt.rgba(32.0/255.0, 57.0/255.0, 77.0/255.0, 1.0)
    visible: false

    Text {
      anchors.fill: parent
      text: "?"
      color: "white"
      horizontalAlignment: TextEdit.AlignHCenter
      font.bold: true
    }

    function showIcon() {
        visible = true;
        helpOverlay.visible = true;
    }

    function showTooltip() {
      helpOverlay.visible = true;
    }

    function hideIcon() {
        hideTooltip();
        visible = false;
    }

    function hideTooltip() {
        helpOverlay.visible = false;
    }

    // Help overlay
    ToolTip {
        id: helpOverlay
        anchors.fill: parent
        width: parent.width
        height: parent.height
        text: helpText
    }

}
