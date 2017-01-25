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

// The menu bar above the memory table for adding new columns and changing the
// representation due to a bug in the qml table header (mouse interaction) it
// wasn't possible to just override the existing table header.
// It is built upon a list of interactive ComboBoxes.
Rectangle {
  id: root
  property var tableView
  property var memoryContent

  height: Theme.memory.header.height
  width: parent.width

  MemoryDivider { anchors.right: parent.right }

  color: Theme.memory.header.background

  Flickable {
    anchors.bottom: parent.bottom
    ListView {
      id: header
      orientation: Qt.Horizontal
      anchors.bottom: parent.bottom
      height: Theme.memory.header.height
      width: tableView.flickableItem.contentWidth

      // Make header follow the flickable
      x: -tableView.flickableItem.contentX

      Connections {
        target: tableView
        onColumnCountChanged: {
          // Dynamically add columns that were added by the user.
          while (headerModel.count < tableView.columnCount) {
            headerModel.append(ListElement);
          }
        }
      }

      model: ListModel { id: headerModel }

      delegate: Rectangle {
        id: headerSection
        height: header.height

        property var currentRole: {
          var column = tableView.getColumn(index);
          return column ? column.role : "";
        }

        onWidthChanged: tableView.getColumn(index).width = width
        width: {
          if (currentRole === 'address') {
            return Theme.memory.address.width;
          } else {
            var remainingWidth = root.width - Theme.memory.address.width;
            var sharedWidth = remainingWidth / (header.count - 1);
            return Math.max(sharedWidth, Theme.memory.cell.minWidth);
          }
        }

        HeaderSection {
          id: headerButton
          anchors.centerIn: parent
        }

        AddColumnButton { visible: index === header.count - 1 }
      }
    }
  }
}
