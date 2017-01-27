/*
* C++ Assembler Interpreter
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import Theme 1.0
import "../../../Js/TextUtility.js" as TextUtility

// Window for input per click settings.
Window {
  id: settingsWindowIC
  width: Theme.input.clickInput.settings.width
  height: Theme.input.clickInput.settings.height

  title: "Click Input Settings"
  flags: Qt.Dialog
  modality: Qt.ApplicationModal

  // Refreshes the window's control contentItem.
  function updateSettings() {
    baseAddressTextField.text = "0x" + inputClickMod.getStart().toString(16);
  }

  onVisibleChanged: settingsWindowIC.updateSettings()

  GridLayout {
    id:grid

    anchors.left: parent.left
    anchors.leftMargin: Theme.output.sevenSegment.settings.margin
    anchors.right: parent.right
    anchors.rightMargin: Theme.output.sevenSegment.settings.margin
    anchors.top: parent.top
    anchors.topMargin: Theme.output.sevenSegment.settings.margin

    columns: 2

    Text {
      text: "Memory Source Address:"
    }

    // Text field for setting the input item's source address in memory.
    TextField {
      id: baseAddressTextField

      onAccepted: { processInput(); }
      onEditingFinished: { processInput(); }

      // Reads the current input and passes the new value to the model.
      function processInput() {
        var inputValue = TextUtility.convertStringToInteger(String(baseAddressTextField.text))
        if (inputValue !== undefined && inputValue >= 0) {
          inputClickMod.setStart(inputValue);
        }
      }
    }
  }

  // Button for accepting setting changes and closing the settings window.
  Button {
    id: doneButton

    text: "Done"

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: Theme.output.sevenSegment.settings.doneButton.bottomMargin

    onClicked: {
      baseAddressTextField.focus = false;
      close();
    }
  }
}
