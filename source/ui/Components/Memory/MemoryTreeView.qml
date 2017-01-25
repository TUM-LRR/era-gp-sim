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

TableView {
  model: memoryModel
  selectionMode: SelectionMode.NoSelection

  // We have a completely custom header (due to a bug in QML).
  headerVisible: false

  property var memoryContent

  style: TableViewStyle {
    transientScrollBars: true
  }

  // The default MemoryView consists of three columns:
  // 1. address                               (fixed)
  // 2. content of each memory cell           (dynamic)
  TableViewColumn {
    role: "address"
    movable: false
    resizable: true
    width: Theme.memory.address.width
  }

  // Add a column with the content for each cell at startup.
  Component.onCompleted: addColumn(memoryContent);
}
