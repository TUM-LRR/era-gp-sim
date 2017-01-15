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
  id: registerTextField

  property bool singleStep: false
  property bool isHighlighted: false
  property color backgroundColor: isHighlighted ? "lightblue" : "white"

  font.pixelSize: Theme.register.content.fontSize

  text: registerContent();

  enabled: model === null || !model.IsConstant

  // Fetches the register's content with the appropiate format from the model.
  function registerContent() {
    if (model === null) return "";
    var registerContentString;
    switch (formatSelector.selection) {
    case "Binary":
      registerContentString = model.BinaryData;
      break;
    case "Hexadecimal":
      registerContentString = model.HexData;
      break;
    case "Unsigned Decimal":
      registerContentString = model.UnsignedDecData;
      break;
    case "Signed Decimal":
      registerContentString = model.SignedDecData;
      break;
    default:
      registerContentString = model.BinaryData;
      break;
    }
    return format(registerContentString);
  }


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
        text = Qt.binding(registerContent);
        if(singleStep) isHighlighted = true;
      }
    }
  }

  // The Registers must know, wether they should be highlighted. They only
  // should change color if execution only works with one line
  Connections {
    target: guiProject
    onRunClicked: {
      registerTextField.singleStep = isSingleStep;
      isHighlighted = false;
    }
  }

  // Notify the model that the register's content was changed by the user.
  onEditingFinished: {
    if (!formatSelector.selection) return;
    registerModel.registerContentChanged(
        styleData.index,
        registerTextField.text,
        formatSelector.selection
    );
  }

  onAccepted: {
    if (!formatSelector.selection) return;
    registerModel.registerContentChanged(
        styleData.index,
        registerTextField.text,
        formatSelector.selection
    );
  }

  Keys.onPressed: {
    if (event.key === Qt.Key_Tab) {
      registerTextField.focus = false;
    }
  }

  function format(registerContentString) {
    registerContentString = registerContentString.replace(/ /g, '');
    if (formatSelector.selection === "Binary"
    || formatSelector.selection === "Hexadecimal") {
      var characterPerByte =
        (formatSelector.selection === "Hexadecimal") ? 2 : 8;
      // Insert new spaces
      for (var characterIndex = 2;
           characterIndex < registerContentString.length;
           characterIndex+=(characterPerByte+1)) {
        registerContentString = [
          registerContentString.slice(0, characterIndex),
          ' ',
          registerContentString.slice(characterIndex)
        ].join('')
      }
    }

    return registerContentString;
  }


  style: TextFieldStyle{
    background: Rectangle {
      id: rect
      x: registerTextField.x
      y: registerTextField.y
      width: registerTextField.width
      height: registerTextField.height
      color: registerTextField.backgroundColor
      radius: 2
      border.color: "lightgray"
      border.width: 1
    }
  }

}
