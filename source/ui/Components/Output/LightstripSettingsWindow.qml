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
import "../Common/TextUtilities.js" as TextUtilities

// Window for lightstrip settings.
Window {
  id: settingsWindow
  width: 350
  height: 120

  title: "Lightstrip Settings"
  flags: Qt.Dialog
  modality: Qt.ApplicationModal

  // Refreshes the window's control contentItem.
  function updateSettings() {
    numberOfStripsTextField.text = outputComponent.getOutputItem(outputItemIndex)["numberOfStrips"];
    baseAddressTextField.text = "0x" + outputComponent.getOutputItem(outputItemIndex)["baseAddress"].toString(16);
  }

  onVisibleChanged: {
    settingsWindow.updateSettings();
  }

  GridLayout {
    id: grid

    anchors.left: parent.left
    anchors.leftMargin: 15
    anchors.right: parent.right
    anchors.rightMargin: 15
    anchors.top: parent.top
    anchors.topMargin: 15

    columns: 2

    Text {
      text: "Memory Source Address:"
    }

    // Text field for setting the output item's source address in memory.
    TextField {
      id: baseAddressTextField

      onAccepted: { processInput(); }
      onEditingFinished: { processInput(); }

      // Reads the current input, checks if it is valid and passes the new value to the model.
      function processInput() {
        var inputValue = TextUtilities.convertStringToInteger(String(baseAddressTextField.text))
        var maxSize = outputComponent.getMemorySize();
        if (inputValue !== undefined && inputValue >= 0 && inputValue < maxSize) {
          outputComponent.setOutputItemProperty(outputItemIndex, "baseAddress", inputValue);
          var maxStrips = (outputComponent.getMemorySize() - (inputValue)) * 8;
          var strips = TextUtilities.convertStringToInteger(String(numberOfStripsTextField.text));
          if(strips > maxStrips){
            numberOfStripsTextField.text = maxStrips + "";
            numberOfStripsTextField.processInput();
          }
        } else {
          updateSettings();
        }
      }
    }

    Text {
      text: "Number of Strips:"
    }

    // Text field for settings the number of light-strips.
    TextField {
      id: numberOfStripsTextField
      height: baseAddressTextField.height

      onAccepted: { processInput(); }
      onEditingFinished: { processInput(); }

      // Reads the current input, checks if it is valid and passes the new value to the model.
      function processInput() {
        var inputValue = TextUtilities.convertStringToInteger(String(numberOfStripsTextField.text));
        var size = TextUtilities.convertStringToInteger(String(baseAddressTextField.text)) ;
        var maxStrips = (outputComponent.getMemorySize() - size) * 8;
        if (inputValue !== undefined && inputValue > 0) {
          if(inputValue <= maxStrips){
            outputComponent.setOutputItemProperty(outputItemIndex, "numberOfStrips", inputValue);
          } else {
            outputComponent.setOutputItemProperty(outputItemIndex, "numberOfStrips", maxStrips);
          }
        } else {
          updateSettings();
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
    anchors.bottomMargin: 5

    onClicked: {
      baseAddressTextField.focus = false;
      numberOfStripsTextField.focus = false;
      close();
    }
  }
}
