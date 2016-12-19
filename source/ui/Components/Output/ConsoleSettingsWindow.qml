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
import QtQuick.Layouts 1.3

// Window for lightstrip settings.
Window {
    id: settingsWindowC
    width: 400
    height: 200
    property int outputItemIndex: 2

    title: "Console Settings"

    // Refreshes the window's control contentItem.
    function updateSettings() {
        baseAddressTextField.text = outputComponent.getOutputItem(outputItemIndex)["baseAddress"];
        var t = outputComponent.getOutputItem(outputItemIndex)["textMode"];
        if(t === 0){
            ab.checked = true;
        } else {
            pl.checked = true;
        }
    }

    // The controls for editing console settings.
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
                text: "Mode:"
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
                    input = input.slice(2);
                    base = 2;
                }
                return parseInt(input, base);
            }

            // Text field for setting the output item's source address in memory.
            TextField {
                id: baseAddressTextField

                onAccepted: { processInput(); }
                onEditingFinished: { processInput(); }

                // Reads the current input and passes the new value to the model.
                function processInput() {
                    var inputValue = controlsColumn.integerFromInputString(String(baseAddressTextField.text));
                    var maxSize = outputComponent.getMemorySize();
                    if (inputValue !== undefined && inputValue >= 0 && inputValue <= maxSize) {
                        outputComponent.setOutputItemProperty(outputItemIndex, "baseAddress", inputValue);
                    }else{
                        updateSettings();
                    }
                }
            }

            // Text field for settings the number of light-strips.
            RowLayout {
                id: mode
                ExclusiveGroup { id: modeGroup }

                RadioButton{
                    id: ab
                    text: "ArrayBased"
                    checked: true
                    exclusiveGroup: modeGroup
                    onClicked: {
                        mode.processInput();
                    }
                }
                RadioButton {
                    id: pl
                    text: "PipeLike"
                    exclusiveGroup: modeGroup
                    onClicked: {
                        mode.processInput();
                    }
                }


                Component.onCompleted: {
                    settingsWindowC.updateSettings();
                }

                // Reads the current input and passes the new value to the model.
                function processInput() {
                    var inputValue = ab.checked? 0 : 1;
                    outputComponent.setOutputItemProperty(outputItemIndex, "textMode", inputValue);

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
            baseAddressTextField.processInput();
            close();
        }
    }
}
