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

Item {
  // number_bits: holds the number of bits shown in each memory cell,
  // it is used for calculating the address and for fetching the right amount of bits from the core
  property int number_bits: 8

  MemoryTreeView {
    id: memoryTreeView
    anchors.top: menuBar.bottom
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    memoryContent: m
  }

  Component {
    id: rowdelegate
    Rectangle {
      height: (styleData.row % (number_bits / 8) == 0) ? 25 : 0
    }
  }

  Component {
    id: itemdelegate
    Label {
      visible: (styleData.row % (number_bits / 8) == 0) ? true : false
      enabled: (styleData.row % (number_bits / 8) == 0) ? true : false
      text: styleData.value
      verticalAlignment: Text.AlignVCenter
    }
  }

  Component {
    id: i
    MemoryCell {
      tableView: memoryTreeView
    }
  }

  MemoryHeader {
    id: menuBar
    tableView: memoryTreeView
    memoryContent: m
  }

  Component {
    id: m
    MemoryContent {
      number_bits: 8
      inputBox: i
    }
  }
}
