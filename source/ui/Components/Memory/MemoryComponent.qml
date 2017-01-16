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
  // numberOfBits: holds the number of bits shown in each memory cell,
  // it is used for calculating the address and for fetching the right amount of bits from the core
  property int numberOfBits: 8
  property int numberOfBytes: numberOfBits / 8

  MemoryTreeView {
    id: memoryTreeView
    memoryContent: content
    itemDelegate: item
    rowDelegate: row
    anchors {
      top: header.bottom
      bottom: parent.bottom
      left: parent.left
      right: parent.right
    }
  }

  Component {
    id: row
    Rectangle {
      height: styleData.row % numberOfBytes ? 0 : 25
    }
  }

  Component {
    id: item
    Label {
      visible: styleData.row % numberOfBytes == 0
      verticalAlignment: Text.AlignVCenter
      text: styleData.value
    }
  }

  Component {
    id: cell
    MemoryCell {
      tableView: memoryTreeView
    }
  }

  MemoryHeader {
    id: header
    tableView: memoryTreeView
    memoryContent: content
  }

  Component {
    id: content
    MemoryContent {
      delegate: cell
    }
  }
}
