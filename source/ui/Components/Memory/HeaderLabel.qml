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

import Theme 1.0

Rectangle {
  color: Theme.memory.header.label.background
  width: Theme.memory.header.label.width
  height: parent.height

  property string comboboxSelection

  Label {
    anchors.fill: parent

    text: {
      if (currentRole === 'address') {
        return Theme.memory.header.label.delimiterOpen
             + comboboxSelection.match(/\d+/)[0]
             + Theme.memory.header.label.delimiterClose;
      } else {
        return Theme.memory.header.label.delimiterOpen
             + currentRole[0]
             + Theme.memory.header.label.delimiterClose;
      }
    }
    verticalAlignment: Qt.AlignVCenter
    color: Theme.memory.header.label.color
    font.pixelSize: Theme.memory.header.label.fontSize
    font.weight: {
      if (Theme.memory.header.label.fontWeight === 'bold') {
        return Font.DemiBold;
      } else {
        return Font.Normal;
      }
    }
  }
}
