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

// The ComboBox above each row the user can either choose the number
// of bits or the numerical representation of a memory cell depending
// on the column.
ComboBox {
  id: combobox
  height: parent.height
  width: Theme.memory.header.button.width

  ContextMenu { target: combobox }

  style: ComboBoxStyle {
    background: Rectangle {
      color: Theme.memory.header.background
    }
    label: Text {
      font.pixelSize: Theme.memory.header.fontSize
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      text: {
        if (currentRole === 'address') {
          var bits = combobox.currentText.match(/\d+/)[0];
          return 'Address (' + bits + ')';
        } else {
          var format = combobox.currentText[0];
          return 'Memory (' + format + ')';
        }
      }
      font.weight: {
        if (Theme.memory.header.fontWeight === 'bold') {
          return Font.DemiBold;
        } else {
          return Font.Normal;
        }
      }
    }
  }

  onWidthChanged: tableView.getColumn(index).width = headerSection.width

  // Choose the right underlying model depending on the
  // column it is responsible for
  model: (currentRole === 'address') ? numberOfBitsModel : formatModel;

  ListModel {
    id: numberOfBitsModel
    ListElement { text: "8 Bit"; bits: 8 }
    ListElement { text: "16 Bit"; bits: 16 }
    ListElement { text: "32 Bit"; bits: 32 }
    ListElement { text: "64 Bit"; bits: 64 }
  }

  ListModel {
    id: formatModel
    ListElement { text: "Binary"; role: "BinaryData" }
    ListElement { text: "Hexadecimal"; role: "HexData" }
    ListElement { text: "Unsigned Decimal"; role: "UnsignedDecimalData" }
    ListElement { text: "Signed Decimal"; role: "SignedDecimalData" }
  }

  onCurrentIndexChanged: {
    if (model === numberOfBitsModel) {
      numberOfBits = model.get(combobox.currentIndex).bits;
    } else {
      tableView.getColumn(index).role = Qt.binding(function() {
        var formatRole = model.get(combobox.currentIndex).role;
        return formatRole + numberOfBits;
      });
    }
  }
}
