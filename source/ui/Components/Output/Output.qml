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


/*this modul is the contaier for the output-windows*/

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "./7-Segment/"

Rectangle {
    id: rootRectangle

    // Color definitions
    property color tabBarColor: Qt.rgba(236.0/255.0, 236.0/255.0, 236.0/255.0, 1.0)
    property color innerBorderColor: "#AFAFAF"
    property color highlightColor: "#4A90E2"
    property color titleColor: "#4A4A4A"
    property color titleColorHighlighted: "#111111"

    // Allows to select the available output views (e.g. Lightstrip, Seven-Segment, Console)
    TabView {
        id: outputTabView

        anchors.fill: parent

        // Position tab bar below the content frame.
        tabPosition: Qt.BottomEdge

        Component.onCompleted: {
//            for (var index = 0; index < outputComponent.getOutputItems().length; ++index) {
//                var currentOutputItem = outputComponent.getOutputItems()[index];
//                var tabComponent;
//                var tabTitle;
//                if (currentOutputItem["type"] == "LightStrip") {
//                    tabComponent = Qt.createComponent("LightStrip.qml");
//                    tabTitle = "Buttons/Lightstrip Icon";
//                } else if (currentOutputItem["type"] == "SevenSegment") {
//                    tabComponent = Qt.createComponent("7-Segment/SevenSegment.qml");
//                    tabTitle = "Buttons/Sevensegment Icon";
//                } else if (currentOutputItem["type"] == "TextConsole") {
//                    tabComponent = Qt.createComponent("RedRectangle.qml");
//                    tabTitle = "Buttons/Text Console Icon";
//                }
//                var tab = outputTabView.addTab(tabTitle, tabComponent);
//                tab.active = true;
//                tab.item.outputItemIndex = index;
//            }
        }

        Tab {
            title: "Buttons/Lightstrip Icon";

            LightStrip {
                outputItemIndex: 0
            }
        }

        Tab {
            title: "Buttons/Sevensegment Icon";

            SevenSegment {
                outputItemIndex: 1
            }
        }

        Tab {
            title: "Buttons/Text Console Icon";

            RedRectangle {
                outputItemIndex: 2
            }
        }



        // Change TabView appearance
        style: TabViewStyle {
            tabBar: Rectangle {
                color: tabBarColor
                // Display border between tab bar and content frame.
                Rectangle {
                    height: 1
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: innerBorderColor
                }

                // Display output settings button in the rightmost corner of the tab bar.
                Button {
                    id: settingsButton
                    anchors.right: parent.right
                    anchors.rightMargin: 4
                    anchors.verticalCenter: parent.verticalCenter
                    height: 18
                    width: 18
                    style: ButtonStyle {
                        background: Rectangle {
                            color: "#00000000"
                            Image {
                                source: (control.pressed) ? "Buttons/Settings Icon Pressed.png" : "Buttons/Settings Icon.png"
                            }
                        }
                    }
                    // Clicking the settings button opens the output settings window.
                    onClicked: {
                        outputTabView.getTab(outputTabView.currentIndex).item.settingsButtonPressed();
                    }
                }
            }

            tab: Rectangle {
                implicitWidth: icon.width + 20
                implicitHeight: 26
                color: Qt.rgba(0, 0, 0, 0)
                Image {
                    id: icon
                    anchors.centerIn: parent
                    // Tab's title contains prefix for icon file; add suffix depending on selection.
                    source: (styleData.selected) ? styleData.title + " Selected.png" : styleData.title + ".png"
                }
            }
        }
    }
}
