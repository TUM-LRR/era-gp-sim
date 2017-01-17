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

  FadeOutEffect { }

  MemoryDivider { anchors.right: parent.right }

  Flickable {
    ListView {
      id: header
      height: parent.height
      width:  tableView.flickableItem.contentWidth
      // move header left and right as the tableView
      // is moved by the horizontal scroll bars
      x: -tableView.flickableItem.contentX

      orientation: Qt.Horizontal

      Connections {
        target: tableView
        onColumnCountChanged: {
          // Dynamically add columns that were added by the user.
          while (headerDropdownList.count < tableView.columnCount - 1) {
            headerDropdownList.append(ListElement);
          }
        }
      }

      model: ListModel { id: headerDropdownList }

      delegate: Rectangle {
        id: headerSection
        property var currentRole: tableView.getColumn(index).role

        height: root.height
        width: {
          if (currentRole === 'address') {
            return Theme.memory.address.width;
          } else {
            return Theme.memory.cell.width;
          }
        }

        property alias text: headerOptions.currentText

        // The ComboBox above each row the user can either choose the number
        // of bits or the numerical representation of a memory cell depending
        // on the column.
        ComboBox {
          id: headerOptions
          height: root.height
          anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: remove.right
            rightMargin: {
              if (currentRole === 'address') {
                return 0;
              } else {
                return Theme.memory.header.remove.marginLeft;
              }
            }
          }

          style: ComboBoxStyle {
            background: Rectangle {
              color: Theme.memory.header.background
            }
            label: Text {
              font.pixelSize: Theme.memory.header.fontSize
              horizontalAlignment: Qt.AlignHCenter
              verticalAlignment: Qt.AlignVCenter
              text: (currentRole === 'address') ? 'Address' : 'Memory'
              font.weight: {
                if (Theme.memory.header.fontWeight === 'bold') {
                  return Font.DemiBold;
                } else {
                  return Font.Normal;
                }
              }
            }
          }

          onWidthChanged: {
            return tableView.getColumn(index).width = headerSection.width;
          }

          // Choose the right underlaying model depending on the
          // column it is responsible for
          model: (currentRole === 'address') ? bitModel : numericModel;

          ListModel {
            id: bitModel
            ListElement { text: "8 Bit"; bits: 8 }
            ListElement { text: "16 Bit"; bits: 16 }
            ListElement { text: "32 Bit"; bits: 32 }
            ListElement { text: "64 Bit"; bits: 64 }
          }

          ListModel {
            id: numericModel
            ListElement { text: "Binary"; role: "bin" }
            ListElement { text: "Hexadecimal"; role: "hex" }
            ListElement { text: "Unsigned Decimal"; role: "dec" }
            ListElement { text: "Signed Decimal"; role: "decs" }
          }

          onCurrentIndexChanged: {
            if (model === bitModel) {
              numberOfBits = model.get(headerOptions.currentIndex).bits;
            } else {
              tableView.getColumn(index).role = Qt.binding(function() {
                var formatRole = model.get(headerOptions.currentIndex).role;
                return formatRole + numberOfBits;
              });
            }
          }
        }

        RemoveButton {
          id: remove
          anchors.right: headerSection.right
          visible: currentRole !== 'address'
          onClicked: {
            tableView.removeColumn(index);
            headerDropdownList.remove(index);
          }
        }
      }
    }
  }
}
