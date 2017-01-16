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

Item {
  property var tableView

  // Makes each memory cell editable by using a textbox
  // when editing is finished the new value is passed to the memory in the core.
  MouseArea {
    hoverEnabled: true
    anchors.fill: parent
    onHoveredChanged: {
      if (containsMouse) {
        cell.borderOpacity = 1;
      } else if (!cell.activeFocus) {
        cell.borderOpacity = 0;
      }
    }

    TextField {
      id: cell
      anchors.right: parent.right
      anchors.verticalCenter: parent.verticalCenter
      horizontalAlignment: Qt.AlignRight

      text: (styleData.row % (numberOfBits / 8) == 0) ? styleData.value : ""

      anchors.fill: parent
      visible: styleData.row % (numberOfBits / 8) == 0
      enabled: styleData.row % (numberOfBits / 8) == 0

      onActiveFocusChanged: {
        cell.borderOpacity = (activeFocus) ? 1 : 0;
      }

      // fadein effect
      property double borderOpacity: 0
      Behavior on borderOpacity {
        NumberAnimation {
          duration: 250
          easing.type: Easing.OutCubic
        }
      }

      onEditingFinished: {
        // update internal memory; use right number representation and byte size
        // if cell is not needed we can save an update
        if(styleData.row % (numberOfBits / 8) !== 0) return;

        memoryModel.setValue(
          styleData.row,
          cell.content,
          numberOfBits,
          tableView.getColumn(styleData.column).role
        );
      }

      style: TextFieldStyle {
        renderType: Text.QtRendering
        font.pixelSize: Theme.memory.cell.fontSize
        textColor: Theme.memory.cell.color
        background: Rectangle {
          color: Theme.memory.cell.background
          TextField {
            anchors.fill: parent
            opacity: cell.borderOpacity
            activeFocusOnTab: false
          }
        }
      }
    } // TextField

    Rectangle {
      anchors {
        bottom: cell.bottom
        right: cell.left
        top: cell.top
      }
      width: Theme.memory.cell.border.width
      color: Theme.memory.cell.border.color
    }

  }
}
