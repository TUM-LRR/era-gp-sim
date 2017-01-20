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

  // Returns a style in the Theme.checkbox or Theme.checkbox.active object
  // depending on whether this checkbox is active. For example,
  // dynamicStyle('foo.bar') will return Theme.checkbox.active.foo.bar when
  // the checkbox is active, and Theme.checkbox.foo.bar otherwise.
  function dynamicStyle(children) {
    var node = checkbox.checked ? Theme.checkbox.active : Theme.checkbox;

    children = children.split('.');
    for (var index = 0; index < children.length; ++index) {
      node = node[children[index]];
    }

    return node;
  }

  style: CheckBoxStyle {
    indicator: Rectangle {
      implicitWidth: Theme.checkbox.width
      implicitHeight: Theme.checkbox.height
      radius: Theme.checkbox.radius
      border.color: dynamicStyle('border.color')
      border.width: dynamicStyle('border.width')
      color: dynamicStyle('background')
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
