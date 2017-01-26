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

ComboBox {
  id: combobox
  height: parent.height
  width: Theme.memory.header.combobox.width

  ContextMenu { }

  anchors {
    bottom: parent.bottom
    bottomMargin: Theme.memory.header.combobox.marginBottom
  }

  style: ComboBoxStyle {
    background: Rectangle {
      color: Theme.memory.header.combobox.background
    }
    label: Text {
      text: currentRole === 'address' ? 'Address' : 'Memory'
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      color: Theme.memory.header.combobox.color
      font.pixelSize: Theme.memory.header.combobox.fontSize
      font.weight: {
        if (Theme.memory.header.combobox.fontWeight === 'bold') {
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
