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

import Theme 1.0

Component {
  Rectangle {
    anchors.centerIn: parent
    color: Theme.memory.address.background

    MemoryDivider { anchors.left: parent.right }

    Label {
      anchors.centerIn: parent
      color: Theme.memory.address.color
      font.family: Theme.memory.address.fontFamily
      font.pixelSize: Theme.memory.address.fontSize
      horizontalAlignment: Text.AlignHCenter
      text: styleData.value
      verticalAlignment: Text.AlignVCenter
      visible: styleData.row % numberOfBytes == 0
      font.weight: {
        if (Theme.memory.address.fontWeight === 'bold') {
          return Font.DemiBold;
        } else {
          return Font.Normal;
        }
      }

    }

    Label {
        id: locklabel
        anchors.right: parent.right
        anchors.rightMargin: 1
        anchors.topMargin: 1
        color: Theme.memory.address.lockColor
        font.pixelSize: Theme.memory.address.lockFontSize
        // create a binding to styleData.value but discard value
        // result: text gets updated properly
        text: styleData.value.substr(0,0) + ((memoryModel.isMemoryProtected(styleData.row))? Theme.memory.address.lockSymbol : "")
    }
  }
}
