/* C++ Assembler Interpreter
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/

import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

import Theme 1.0

// This rectangle is only used for styling it provides a fadeout effect on
// the right side of the header when there are too many columns the
// ComboBoxes slightly disappear with a fadeout effect on the right.

Rectangle {
  width: Theme.memory.addButton.width
  height: Theme.memory.addButton.height
  anchors {
    right: parent.right
    bottom: parent.bottom
    bottomMargin: Theme.memory.addButton.marginBottom
    top: parent.top
  }

  color: Theme.memory.addButton.background
  LinearGradient{
    anchors.fill: parent
    start: Qt.point(0, 0)
    end: Qt.point(parent.width, 0)

    gradient: Gradient {
      GradientStop { position: 0.0; color: "#00000000" }
      GradientStop { position: 0.4; color: "white" }
    }
  }

  Button {
    id: button
    anchors {
      right: parent.right
      bottom: parent.bottom
      top: parent.top
    }

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.PointingHandCursor
      onClicked: {
        tableView.insertColumn(tableView.columnCount - 1, memoryContent);
      }
    }

    style: ButtonStyle {
      background: Rectangle { visible: false }
      label: Text {
        text: "+"
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        color: Theme.memory.addButton.color
        font.pixelSize: Theme.memory.addButton.fontSize
        font.weight: {
          if (Theme.memory.header.fontWeight === 'bold') {
            return Font.DemiBold;
          } else {
            return Font.Normal;
          }
        }
      }
    }
  }
}
