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

// The menu bar above the memory table for adding new columns and changing the
// representation due to a bug in the qml table header (mouse interaction) it
// wasn't possible to just override the existing table header.
// It is built upon a list of interactive ComboBoxes.
Rectangle {
  property var tableView
  property var memoryContent

  height: 25
  width: parent.width

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
          // dynamically add columns that were added by the user
          while(headerDropdownList.count < tableView.columnCount - 1) {
            headerDropdownList.append(ListElement);
          }
        }
      }

      model: ListModel {
        id: headerDropdownList
      }

      delegate: Rectangle {
        // part of the header for every single column
        // consists of a ComboBox and a rectangle for resizing the column width

        width: bitChooser.width + resizer.width
        height: 25

        property alias text: bitChooser.currentText

        ComboBox {
          // the ComboBox above each row the user can either choose the number
          // of bits or the numberical representation of a memory cell depending
          // on the column

          id: bitChooser
          height: 25

          // bin width to the position of resizer
          width: resizer.x - bitChooser.x
          onWidthChanged: {
            tableView.getColumn(index).width = bitChooser.width + resizer.width;
          }

          // Choose the right underlaying model depending on the
          // column it is responsible for
          model: (tableView.getColumn(index).role === "address")? modelBits : modelNumeric;

          ListModel {
            id: modelBits
            ListElement { text: "8 Bit"; bits: 8 }
            ListElement { text: "16 Bit"; bits: 16 }
            ListElement { text: "32 Bit"; bits: 32 }
            ListElement { text: "64 Bit"; bits: 64 }
          }
          ListModel {
            id: modelNumeric
            ListElement { text: "Binary"; role: "bin" }
            // ListElement { text: "Octal"; role: "oct" } // not supported
            ListElement { text: "Hexadecimal"; role: "hex" }
            ListElement { text: "Decimal"; role: "dec" }
            ListElement { text: "Decimal (signed)"; role: "decs" }

            ListElement { text : "remove..." }
          }

          onCurrentIndexChanged: {
            // depending on the usage there are 3 different actions
            // 1. update the number of bits in each memory cell
            if(model === modelBits) {
              numberOfBits = model.get(bitChooser.currentIndex).bits;
            }
            else {
              // 2. dynamically remove the column
              if(bitChooser.currentText === "remove...") {
                tableView.removeColumn(index);
                headerDropdownList.remove(index);
              }
              // 3. update the numeric representation of the memory values
              else {
                // explicitly create a property binding for numberOfBits so the role gets updated correctly
                tableView.getColumn(index).role = Qt.binding(function() {
                  return model.get(bitChooser.currentIndex).role + numberOfBits })
                }
              }
            }
          }

          Rectangle {
            // This rectangle is a resizer located next to each ComboBox in the
            // header of the memory. by dragging this rectangle to the left or
            // right someone could resize the width of each column
            id: resizer
            height: 25
            width: 5
            x: bitChooser.x + 70 // default width of a column
            MouseArea {
              drag.axis: Drag.XAxis
              drag.target: resizer
              anchors.fill: parent
              cursorShape: Qt.SizeHorCursor
              //give a minimum size for column width
              drag.minimumX: bitChooser.x + 40
            }
          }
        }
      }
    }

    Rectangle {
      // This rectangle is only used for styling it provides a fadeout effect on
      // the right side of the header when there are too many columns the
      // ComboBoxes slightly disappear with a fadeout effect on the right
      id: buttonFadeOut
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
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.rightMargin: 1
        anchors.topMargin: 1
        anchors.bottomMargin: 1
        width: 25

        text: "+"

        onClicked: {
          tableView.insertColumn(tableView.columnCount - 1, memoryContent);
        }
      }
    }
  }
