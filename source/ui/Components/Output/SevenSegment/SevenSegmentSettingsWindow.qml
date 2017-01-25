
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

/*
  a 7-segment digit for displaying numbers:
   _   _
  |_| |_|
  |_| |_|

*/

import QtQuick 2.6
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

// Window for seven segment settings.
Window {
    id: settingsWindow
    width: 400
    height: 200

    title: "Seven-Segment Settings"

    // Refreshes the window's control content.
    function updateSettings() {
        numberOfDigitsTextField.text = outputComponent.getOutputItem(outputItemIndex)["numberOfDigits"];
        baseAddressTextField.text = outputComponent.getOutputItem(outputItemIndex)["baseAddress"];
    }

    // The controls for editing seven segment settings.
    Row {
        anchors.fill: parent
        anchors.leftMargin: 15
        anchors.topMargin: 15
        anchors.rightMargin: 15
        anchors.bottomMargin: 15

        spacing: 15

        // Title of each settings control.
        Column {
            spacing: 16
            Text {
                text: "Memory Source (Address):"
            }
            Text {
                text: "Number of Digits:"
            }
        }

        // Settings controls itself.
        Column {
            id: controlsColumn
            spacing: 8

            // Converts a given dec/hex/bin-string to an integer.
            function integerFromInputString(input) {
                var base = 10;
                if (input.indexOf("0x") === 0) {
                    base = 16;
                    input = input.slice(2);
                } else if (input.indexOf("0b") === 0) {
                    base = 2;
                    input = input.slice(2);
                }
                return parseInt(input, base);
            }

            // Text field for setting the output item's source address in memory.
            TextField {
                id: baseAddressTextField

                text: outputComponent.getOutputItem(outputItemIndex)["baseAddress"]

                onAccepted: { processInput(); }
                onEditingFinished: { processInput(); }

                // Reads the current input and passes the new value to the model.
                function processInput() {
                    var inputValue = controlsColumn.integerFromInputString(String(baseAddressTextField.text))
                    var maxSize = outputComponent.getMemorySize();
                    if (inputValue !== undefined && inputValue >= 0 && inputValue < maxSize) {
                        // Check if there are too many digits to fit into memory for the
                        // currently set number of digits. Adjust number of digits if necessary.
                        var maxDigits = (maxSize - inputValue);
                        var digits = controlsColumn.integerFromInputString(String(numberOfDigitsTextField.text));
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
                    var inputValue = controlsColumn.integerFromInputString(String(numberOfDigitsTextField.text));
                    if (inputValue !== undefined && inputValue > 0) {
                        // Check if the new number of digits would be too high to fit into memoryModel
                        // and adjust number to maximum value if necessary.
                        var maxSize = outputComponent.getMemorySize();
                        var maxDigits = maxSize - controlsColumn.integerFromInputString(String(baseAddressTextField.text));
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
    }

    // Button for accepting setting changes and closing the settings window.
    Button {
        id: doneButton

        text: "Done"

        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5

        onClicked: {
            baseAddressTextField.focus = false;
            numberOfDigitsTextField.focus = false;
            close();
        }
    }
}
