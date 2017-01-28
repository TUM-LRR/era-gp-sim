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

CheckBox {
  id: checkbox
  checked: true

  property string text
  property var theme: ({})

  MouseArea {
    anchors.fill: parent
    cursorShape: Qt.PointingHandCursor
    onClicked: checked = !checked
  }

  property var dynamic: ({ theme: ThemeUtility.dynamicThemeFactory(
    Theme.checkbox,
    checkbox.checked,
    'active'
  )})

  style: CheckBoxStyle {
    indicator: Rectangle {
      implicitWidth: Theme.checkbox.width
      implicitHeight: Theme.checkbox.height
      radius: Theme.checkbox.radius
      border.color: dynamic.theme('border.color')
      border.width: dynamic.theme('border.width')
      color: dynamic.theme('background')
    }

    label: Text {
      text: checkbox.text
      leftPadding: Theme.checkbox.margin
      color: theme.color || Theme.checkbox.color
      font.pixelSize: theme.fontSize || Theme.checkbox.fontSize
      font.weight: {
        var fontWeight = theme.fontWeight || Theme.checkbox.fontWeight;
        return fontWeight === 'bold' ? Font.DemiBold : Font.Normal;
      }
    }
  }
}
