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
    anchors.fill: parent
    hoverEnabled: true
    onHoveredChanged: {
      if (containsMouse) {
        cell.borderOpacity = 1;
      } else if (!cell.activeFocus) {
        cell.borderOpacity = 0;
      }
    }

    MemoryBorder { }

    TextField {
      id: cell
      horizontalAlignment: Qt.AlignRight
      anchors {
        fill: parent
        right: parent.right
        verticalCenter: parent.verticalCenter
      }

      enabled: styleData.row % numberOfBytes == 0
      visible: enabled

      text: enabled ? styleData.value : ""

      onActiveFocusChanged: cell.borderOpacity = (activeFocus) ? 1 : 0;

      // Fadein effect
      property double borderOpacity: 0
      Behavior on borderOpacity {
        NumberAnimation {
          duration: 250
          easing.type: Easing.OutCubic
        }
      }

      onEditingFinished: {
        if(!enabled) return;
        memoryModel.setValue(
          styleData.row,
          cell.content,
          numberOfBits,
          currentRole
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
  }
}
