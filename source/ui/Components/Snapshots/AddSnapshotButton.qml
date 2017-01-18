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

import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Theme 1.0

Button {
  id: button
  text: "Save Snapshot"
  anchors {
    right: parent.right
    top: parent.top
    bottom: parent.bottom
    bottomMargin: Theme.snapshots.button.marginBottom
  }

  MouseArea {
    anchors.fill: parent
    cursorShape: Qt.PointingHandCursor
    onClicked: button.clicked()
  }

  style: ButtonStyle {
    background: Rectangle {
      color: Theme.snapshots.button.background
      height: Theme.snapshots.title.height
    }
    label: Text {
      text: "+"
      color: Theme.snapshots.button.color
      font.pixelSize: Theme.snapshots.button.fontSize
      verticalAlignment: Text.AlignVCenter
      font.bold: {
        if (Theme.snapshots.title.fontWeight === 'bold') {
          return Font.DemiBold;
        } else {
          return Font.Normal;
        }
      }
    }
  }
}
