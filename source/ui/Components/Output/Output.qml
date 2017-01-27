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
import Theme 1.0
import "./SevenSegment/"
import "./Lightstrip/"
import "./Console/"

/*
 Container for output items (e.g. lightstrip, seven-segment, text console).
 */
Item {
    id: rootRectangle

    // Color definitions
    property color tabBarColor: Theme.output.tabBar.background
    property color innerBorderColor: Theme.output.tabBar.border.color

    // Tell SplitViewItem (i.e. component wrapper) that settings are available to make it display settings icon.
    property var hasComponentSettings: true

    // Allows to select the available output items (e.g. Lightstrip, Seven-Segment, Console)
    TabView {
        id: outputTabView

        anchors.fill: parent

        // Position tab bar below the content frame.
        tabPosition: Qt.BottomEdge


        /* Each output item is represented by its corresponding tab inside the output tab bar.
           Every output item needs the following properties to be able to connect with the output model:
           - outputItemIndex: Unique index identifying each output item. Has to correspond with the item's
             index inside the _outputItemsInformation-array of the output model (refer to output-component.hpp).
           - settingsButtonPressed(): Signal for notifying the output item that its settigns button was pressed and
             that it should therefore display its settings menu. The settings button itself is part of the tab bar
             and not the output item itself.
        */
        Tab {
            title: "Buttons/Lightstrip Icon"

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
            title: "Buttons/Text Console Icon"
            ConsoleComponent {
                outputItemIndex: 2
            }
        }



        // Change TabView appearance
        style: TabViewStyle {
            tabBar: Rectangle {
                color: tabBarColor
                // Display border between tab bar and content frame.
                Rectangle {
                    height: Theme.output.tabBar.border.width
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: innerBorderColor
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

    // Called by SplitViewItem.qml (i.e. component wrapper) when component settings icon was pressed.
    function settingsButtonPressed() {
        outputTabView.getTab(outputTabView.currentIndex).item.settingsButtonPressed();
    }
}
