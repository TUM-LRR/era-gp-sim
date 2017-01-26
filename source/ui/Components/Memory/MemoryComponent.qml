/* ERASIM Assembler Interpreter
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

Item {
  // numberOfBits: holds the number of bits shown in each memory cell,
  // it is used for calculating the address and for fetching the right amount of bits from the core
  property int numberOfBits: 8
  property int numberOfBytes: numberOfBits / 8

  MemoryHeader {
    id: header
    tableView: memoryTreeView
    memoryContent: content
  }

  MemoryTreeView {
    id: memoryTreeView
    memoryContent: content
    itemDelegate: MemoryAddressCell { }
    rowDelegate: row
    anchors {
      top: header.bottom
      bottom: parent.bottom
      right: parent.right
      left: parent.left
    }
  }

  Component {
    id: row
    Rectangle {
      height: styleData.row % numberOfBytes ? 0 : Theme.memory.cell.height
    }
  }

  Component {
    id: content
    MemoryContent {
      delegate: Component {
        MemoryCell { tableView: memoryTreeView }
      }
    }
  }

}
