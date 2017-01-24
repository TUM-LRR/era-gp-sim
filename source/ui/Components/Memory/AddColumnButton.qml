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

Button {
  id: button
  anchors {
    top: parent.top
    bottom: parent.bottom
    bottomMargin: Theme.memory.add.marginBottom
    right: parent.right
    rightMargin: Theme.memory.add.marginRight
  }

  MouseArea {
    anchors.fill: parent
    cursorShape: Qt.PointingHandCursor
    onClicked: tableView.addColumn(memoryContent);
  }

  style: ButtonStyle {
    background: Rectangle { color: Theme.memory.add.background }
    label: Text {
      text: "+"
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      color: Theme.memory.add.color
      font.pixelSize: Theme.memory.add.fontSize
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
