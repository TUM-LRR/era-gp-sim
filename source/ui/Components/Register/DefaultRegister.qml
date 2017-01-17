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
* along with this program. If not, see http://www.gnu.org/licenses/.*/

import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import Theme 1.0

TextField {
  id: root
  enabled: model === null || !model.IsConstant

  property bool singleStep: false
  property bool isHighlighted: false

  font.pixelSize: Theme.register.content.fontSize
  horizontalAlignment: TextInput.AlignRight
  text: getRegisterContent()

  // As some values need to be set manually (i.e. not using the model's
  // data-method and the corresponding roles), they also have to be updated
  // manually whenever the model's data changed for the current index.
  Connections {
    target: registerModel
    // Data changed is emitted when some of the model's data has changed (e.g.
    // refer to `updateContent`- method).
    onDataChanged: {
      // Check if the current item's index is affected by the data change.
      if (topLeft <= styleData.index && styleData.index <= bottomRight) {
        text = Qt.binding(getRegisterContent);
        if(singleStep) isHighlighted = true;
      }
    }
  }

  // The Registers must know, wether they should be highlighted. They only
  // should change color if execution only works with one line
  Connections {
    target: guiProject
    onRunClicked: {
      root.singleStep = isSingleStep;
      isHighlighted = false;
    }
  }

  // Notify the model that the register's content was changed by the user.
  onEditingFinished: {
    if (!formatSelector.selection) return;
    registerModel.registerContentChanged(
      styleData.index,
      root.text,
      formatSelector.selection
    );
  }

  onAccepted: {
    if (!formatSelector.selection) return;
    registerModel.registerContentChanged(
      styleData.index,
      root.text,
      formatSelector.selection
    );
  }

  Keys.onPressed: {
    if (event.key === Qt.Key_Tab) {
      root.focus = false;
    }
  }

  style: TextFieldStyle {
    textColor: Theme.register.content.color
    background: Rectangle {
      height: Theme.register.content.height
      radius: Theme.register.content.radius
      border.color: Theme.register.content.border.color
      border.width: Theme.register.content.border.width
      color: {
        if (root.isHighlighted) {
          return Theme.register.content.highlighted.background;
        } else {
          return Theme.register.content.background;
        }
      }
    }
  }

  function insertWhitespace(content) {
    // For binary and hex numbers we insert a space
    // every 4/2 characters (to delimit a nibble/byte)
    if (formatSelector.selection === 'Binary') {
      return content.replace(/(\d{4})(?=.)/g, '$& ');
    } else if (formatSelector.selection === 'Hexadecimal') {
      return content.replace(/([\da-fA-F]{2})(?=.)/g, '$& ');
    } else {
      return content;
    }
  }

  function getRegisterContent() {
    if (model === null) return "";
    switch (formatSelector.selection) {
    case "Binary":
      return insertWhitespace(model.BinaryData);
    case "Hexadecimal":
      return insertWhitespace(model.HexData);
    case "Unsigned Decimal":
      return insertWhitespace(model.UnsignedDecimalData);
    default:
      return insertWhitespace(model.SignedDecimalData);
    }
  }

}
