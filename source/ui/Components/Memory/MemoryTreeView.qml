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

TableView {
  property var memoryContent

  selectionMode: SelectionMode.NoSelection
  verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

  // we have a completly custom header (due to a bug in qml)
  headerVisible: false

  // the default MemoryView consists of three columns:
  // 1. address                               (fixed)
  // 2. content of each memory cell           (dynamic)
  // 3. additional information on each cell   (fixed)
  TableViewColumn {
    role: "address"
    title: "Address"
    movable: false
    resizable: true
    width: 70
  }
  TableViewColumn {
    role: "info"
    title: "Info"
    movable: false
    resizable: true
    width: 100
  }

  model: memoryModel
  rowDelegate: rowdelegate
  itemDelegate: itemdelegate

  // add a column with the content for each cell at startup
  Component.onCompleted: {
    insertColumn(columnCount - 1, memoryContent);
  }
}
