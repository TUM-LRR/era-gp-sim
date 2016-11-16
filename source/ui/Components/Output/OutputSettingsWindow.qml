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
import "../Common"

/**
 Window displaying settings for all the output components.
 */
Window {
    width: 400
    height: 200

    title: "Output Settings"

    property alias activeComponentIndex: tabView.currentIndex
    property int lightStripBaseAddress: 0
    property int lightStripNumberOfStrips: 8

    TabView {
        id: tabView

        anchors.fill: parent

        // Settings for the lightstrip component.
        Tab {
            title: "Lighstrip"

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
                    spacing: 8

                    // Text field for setting the component's source address in memory.
                    TextField {
                        id: lightstripMemorySourceTextField
                        text: "0x ABCDEF01"

                        onTextChanged: {
                            // TODO...
                            lightStripBaseAddress = 8;
                        }
                    }

                    // Numeric control for setting the number of strips.
                    NumericUpDown{
                        id: stripCountNumUpDown

                        height: lightstripMemorySourceTextField.height
                        _step: 1
                        _value: lightStripNumberOfStrips
                        _minValue: 0
                        _maxValue: 16
                        onValueChanged: {
                            lightStripNumberOfStrips = _value;
                        }
                    }
                }
            }

        }


        // Settings for the sevensegment component.
        Tab {
            title: "Sevensegment"

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
                        text: "Number of Digits:"
                    }
                }

                // Settings controls itself.
                Column {
                    spacing: 8

                    // Text field for setting the component's source address in memory.
                    TextField {
                        id: sevensegmentMemorySourceTextField
                        text: "0x ABCDEF01"
                    }

                    // Numeric control for setting the number of digits.
                    NumericUpDown{
                        id: digitCountNumUpDown

                        height: sevensegmentMemorySourceTextField.height
                        _step: 1
                        _value: 1
                        _minValue: 1
                        _maxValue: 8
                    }
                }
            }
        }


        // Settings for the text console component.
        Tab {
            title: "Text Console"

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
                        text: "Input Mode:"
                    }
                }
                // Settings controls itself.
                Column {
                    spacing: 8

                    // Text field for setting the component's source address in memory.
                    TextField {
                        id: textConsoleMemorySourceTextField
                        text: "0x ABCDEF01"
                    }

                    // Segmented control for setting the text console's input mode.
                    Item {
                        id: segmentedControl
                        height: buttonRow.height
                        width: buttonRow.width

                        property int currentIndex: 0
                        Row {
                            id: buttonRow
                            height: buttonOne.height
                            Button {
                                id: buttonOne
                                text: "Pipe-Like"
                                style: ButtonStyle {
                                    background: Rectangle {
                                        color: (segmentedControl.currentIndex == 0) ? "#4A90E2" : "white"
                                        border.color: "#EFEDEF"
                                    }
                                    label: Text {
                                        text: buttonOne.text
                                        color: (segmentedControl.currentIndex == 0) ? "white" : "black"
                                    }
                                }
                                onClicked: {
                                    segmentedControl.currentIndex = 0;
                                }
                            }
                            Button {
                                id: buttonTwo
                                text: "Array-Based"
                                style: ButtonStyle {
                                    background: Rectangle {
                                        color: (segmentedControl.currentIndex == 1) ? "#4A90E2" : "white"
                                        border.color: "#EFEDEF"
                                    }
                                    label: Text {
                                        text: buttonTwo.text
                                        color: (segmentedControl.currentIndex == 1) ? "white" : "black"
                                    }
                                }
                                onClicked: {
                                    segmentedControl.currentIndex = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
