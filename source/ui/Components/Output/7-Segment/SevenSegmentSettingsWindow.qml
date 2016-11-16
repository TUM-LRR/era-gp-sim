
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

import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

// Window for sevent segment settings.
Window {
    id: settingsWindow
    width: 400
    height: 200

    title: "Seven-Segment Settings"

    function updateSettings() {
        numberOfStripsTextField.text = outputComponent.getOutputItems()[outputItemIndex]["numberOfDigits"];
        baseAddressTextField.text = outputComponent.getOutputItems()[outputItemIndex]["baseAddress"];
    }

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
                text: "Memory Source:"
            }
            Text {
                text: "Number of Strips:"
            }
        }

        // Settings controls itself.
        Column {
            id: controlsColumn
            spacing: 8

            function integerFromInputString(input) {
                var base = 10;
                if (input.indexOf("0x") == 0) {
                    base = 16;
                    input = input.slice(2);
                } else if (input.indexOf("0b") == 0) {
                    input = input.slice(2);
                    base = 2;
                }
                return parseInt(input, base);
            }

            // Text field for setting the output item's source address in memory.
            TextField {
                id: baseAddressTextField

                text: outputComponent.getOutputItems()[outputItemIndex]["baseAddress"]

                onAccepted: { processInput(); }
                onEditingFinished: { processInput(); }

                function processInput() {
                    var inputValue = controlsColumn.integerFromInputString(String(baseAddressTextField.text))
                    if (inputValue !== undefined && inputValue >= 0) {
                        outputComponent.setOutputItemProperty(outputItemIndex, "baseAddress", inputValue);
                    }
                }
            }

            TextField {
                id: numberOfStripsTextField
                height: baseAddressTextField.height

                text: numberOfStripsTextField.text = outputComponent.getOutputItems()[outputItemIndex]["numberOfDigits"];

                onAccepted: { processInput(); }
                onEditingFinished: { processInput(); }

                Component.onCompleted: {
                    settingsWindow.updateSettings();
                }

                function processInput() {
                    var inputValue = controlsColumn.integerFromInputString(String(numberOfStripsTextField.text));
                    if (inputValue !== undefined && inputValue > 0) {
                        outputComponent.setOutputItemProperty(outputItemIndex, "numberOfDigits", inputValue);
                    }
                }
            }

        }
    }
}
