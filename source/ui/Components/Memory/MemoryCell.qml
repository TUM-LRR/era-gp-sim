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

Component {
  // makes each memory cell editable by using a textbox
  // when editing is finished the new value is passed to the memory in the core
  MouseArea {
    hoverEnabled: true
    anchors.fill: parent
    // fadein on mouse hover
    onHoveredChanged: {
      if(containsMouse) {
        textFieldMemoryValue.borderopacity = 1;
      } else if(!textFieldMemoryValue.activeFocus) {
        textFieldMemoryValue.borderopacity = 0;
      }
    }

    TextField {
      id: textFieldMemoryValue
      text: (styleData.row % (number_bits / 8) == 0) ? styleData.value : ""
      anchors.fill: parent
      visible: (styleData.row % (number_bits / 8) == 0) ? true : false
      enabled: (styleData.row % (number_bits / 8) == 0)

      onActiveFocusChanged: {
        textFieldMemoryValue.borderopacity = (activeFocus) ? 1 : 0;
      }

      // fadein effect
      property double borderopacity: 0
      Behavior on borderopacity {
        NumberAnimation {
          duration: 250
          easing.type: Easing.OutCubic
        }
      }

      onEditingFinished: {
        // update internal memory; use right number representation and byte size
        // if cell is not needed we can save an update
        if(styleData.row % (number_bits / 8) == 0) {
          memoryModel.setValue(
            styleData.row,
            textFieldMemoryValue.text,
            number_bits,
            tableView.getColumn(styleData.column).role
          );
        }
      }

      style: TextFieldStyle {
        id: style
        renderType: Text.QtRendering
        background: Item{
          // textfield as background for default look and feel
          TextField {
            id: background
            anchors.fill: parent
            opacity: textFieldMemoryValue.borderopacity
            activeFocusOnTab: false
          }
        }
      }
    }
  }
}
