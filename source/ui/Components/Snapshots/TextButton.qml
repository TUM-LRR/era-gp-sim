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

Button {
  id: button
  property var theme
  property bool highlighted

  anchors {
    right: parent.right
    rightMargin: theme.rightMargin
    top: parent.top
    topMargin: theme.topMargin
    bottom: parent.bottom
    bottomMargin: theme.marginBottom
  }

  MouseArea {
    anchors.fill: parent
    cursorShape: Qt.PointingHandCursor
    onClicked: button.clicked()
  }

  style: ButtonStyle {
    background: Rectangle {
      color: theme.background
      height: theme.height
    }
    label: Text {
      text: button.text
      color: theme.color
      font.pixelSize: theme.fontSize
      verticalAlignment: Text.AlignVCenter
      wrapMode: Text.Wrap
      elide: Text.ElideRight
      maximumLineCount: 1
      font.weight: {
        if (highlighted || theme.fontWeight === 'bold') {
          return Font.DemiBold;
        } else {
          return Font.Normal;
        }
      }
    }
  }
}
