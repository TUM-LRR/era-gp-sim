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

import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import Theme 1.0
import "../../../Js/TextUtility.js" as TextUtility

// Window for seven segment settings.
Window {
  id: settingsWindow
  width: Theme.output.lightStrip.settings.width
  height: Theme.output.lightStrip.settings.height

  title: "Seven-Segment Settings"
  flags: Qt.Dialog
  modality: Qt.ApplicationModal

  // Refreshes the window's control content.
  function updateSettings() {
    numberOfDigitsTextField.text = outputComponent.getOutputItem(outputItemIndex)["numberOfDigits"];
    baseAddressTextField.text = "0x" + outputComponent.getOutputItem(outputItemIndex)["baseAddress"].toString(16);
  }

  onVisibleChanged: settingsWindow.updateSettings();

  GridLayout {
    id: grid

    anchors.left: parent.left
    anchors.leftMargin: Theme.output.lightStrip.settings.margin
    anchors.right: parent.right
    anchors.rightMargin: Theme.output.lightStrip.settings.margin
    anchors.top: parent.top
    anchors.topMargin: Theme.output.lightStrip.settings.margin

    columns: 2

    Text {
      text: "Memory Source Address:"
    }

    // Text field for setting the output item's source address in memory.
    TextField {
      id: baseAddressTextField

      text: outputComponent.getOutputItem(outputItemIndex)["baseAddress"]

      onAccepted: processInput()
      onEditingFinished: processInput()

      // Reads the current input and passes the new value to the model.
      function processInput() {
        var inputValue = TextUtility.convertStringToInteger(String(baseAddressTextField.text))
        var maxSize = outputComponent.getMemorySize();
        if (inputValue !== undefined && inputValue >= 0 && inputValue < maxSize) {
          // Check if there are too many digits to fit into memory for the
          // currently set number of digits. Adjust number of digits if necessary.
          var maxDigits = (maxSize - inputValue);
          var digits = TextUtility.convertStringToInteger(String(numberOfDigitsTextField.text));
          if (digits > maxDigits) {
            numberOfDigitsTextField.text = maxDigits + "";
            numberOfDigitsTextField.processInput();
          }
          // Update to new value.
          outputComponent.setOutputItemProperty(outputItemIndex, "baseAddress", inputValue);
        } else {
          // If incorrect value was entered, reset to previous value.
          updateSettings();
        }
      }
    }

    Text {
      text: "Number of Digits:"
    }

    // Text field for settings the number of digits.
    TextField {
      id: numberOfDigitsTextField
      height: baseAddressTextField.height

      text: numberOfDigitsTextField.text = outputComponent.getOutputItem(outputItemIndex)["numberOfDigits"];

      onAccepted: { processInput(); }
      onEditingFinished: { processInput(); }

      Component.onCompleted: {
        settingsWindow.updateSettings();
      }

      // Reads the current input and passes the new value to the model.
      function processInput() {
        var inputValue = TextUtility.convertStringToInteger(String(numberOfDigitsTextField.text));
        if (inputValue !== undefined && inputValue > 0) {
          // Check if the new number of digits would be too high to fit into memoryModel
          // and adjust number to maximum value if necessary.
          var maxSize = outputComponent.getMemorySize();
          var maxDigits = maxSize - TextUtility.convertStringToInteger(String(baseAddressTextField.text));
          if (inputValue > maxDigits) {
            inputValue = maxDigits;
          }
          // Update to new value.
          outputComponent.setOutputItemProperty(outputItemIndex, "numberOfDigits", inputValue);
        } else {
          // If incorrect value was entered, reset to previous value.
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
    anchors.bottomMargin: Theme.output.lightStrip.settings.doneButton.bottomMargin

    onClicked: {
      baseAddressTextField.focus = false;
      numberOfDigitsTextField.focus = false;
      close();
    }
  }
}
