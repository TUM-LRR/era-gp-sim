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

import Theme 1.0
import "../../../Js/ThemeUtility.js" as ThemeUtility
import ".."


Button {
  id: button
  enabled: false
  anchors {
    horizontalCenter: parent.horizontalCenter
    topMargin: Theme.createProject.button.marginTop
    bottomMargin: 20
  }

  MouseArea {
    anchors.fill: parent
    cursorShape: Qt.PointingHandCursor
    onClicked: button.clicked()
  }

  property var dynamic: ({ theme: ThemeUtility.dynamicThemeFactory(
      Theme.button,
      !button.enabled,
      'disabled'
  )})

  style: ButtonStyle {
    label: Item {
      anchors.centerIn: parent
      Text {
        anchors.centerIn: parent
        color: dynamic.theme('color')
        font.pixelSize: Theme.button.fontSize
        font.capitalization: Font.AllUppercase
        text: "Create project"
      }
    }
    background: Rectangle {
      implicitWidth: Theme.createProject.button.width
      implicitHeight: Theme.button.height
      border.width: Theme.button.border.width
      border.color: dynamic.theme('border.color')
      radius: Theme.button.radius
      color: {
        if (button.enabled) {
          return Theme.createProject.button.background;
        } else {
          return Theme.button.disabled.background;
        }
      }
    }
  }
}
