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
import Theme 1.0

Rectangle {
  id: root
  anchors.top: parent.top

  color: Theme.input.clickInput.background

  property var annotationMargin: Theme.input.clickInput.clickArea.margin
  property var annotationColor: Theme.input.clickInput.clickArea.annotation.color

  Rectangle {
    id: area
    anchors.fill: parent
    anchors.margins: root.annotationMargin

    color: Theme.input.clickInput.clickArea.background

    MouseArea {
      id: mouseArea
      anchors.fill: parent

      onClicked: {
        inputClickMod.newClick(mouse.x/mouseArea.width * 255,
                               mouse.y/mouseArea.height * 255);
      }
    }
  }

  Item {
    id: xRect
    height: annotationMargin
    width: area.width
    anchors.left: area.left
    anchors.top: parent.top

    Text {
      anchors.bottom: parent.bottom
      anchors.left: parent.left
      text: "0"
      color: annotationColor
    }

    Text {
      font.pointSize: 12
      text: "x\n\u2192" // Right arrow
      anchors.centerIn: parent
      color: annotationColor
    }

    Text {
      anchors.bottom: parent.bottom
      anchors.right: parent.right
      text: "255"
      color: annotationColor
    }
  }

  Item  {
    width: annotationMargin
    height: area.height
    anchors.right: area.left
    anchors.top: area.top

    Text {
      anchors.top: parent.top
      anchors.right: parent.right
      text: "0  "
      color: annotationColor
    }

    Text {
      font.pointSize: 12
      anchors.centerIn: parent
      text: "y \u2193"  // Down arrow
      color: annotationColor
    }

    Text {
      anchors.bottom: parent.bottom
      anchors.right: parent.right
      text: "255  "
      color: annotationColor
    }
  }

  function settingsButtonPressed() {
    settingsWindowIC.show();
  }

  ClickInputSettings {
    id: settingsWindowIC
  }

}
