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
// ComboBoxes slightly disappear with a fadeout effect on the right

Rectangle {
  width: 50
  height: 25
  anchors.right: parent.right
  color: "transparent"
  LinearGradient{
    anchors.fill: parent
    start: Qt.point(0, 0)
    end: Qt.point(parent.width, 0)

    gradient: Gradient {
      GradientStop { position: 0.0; color: "#00000000" }
      GradientStop { position: 0.4; color: "white" }
    }
  }

  // This button is used for creating new columns for the memory.
  Button {
    anchors {
      right: parent.right
      top: parent.top
      bottom: parent.bottom
      rightMargin: 1
      topMargin: 1
      bottomMargin: 1
    }
    width: 25

    text: "+"

    onClicked: {
      tableView.insertColumn(tableView.columnCount - 1, memoryContent);
    }
  }
}
