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



/*this modul is the contaier for the input-windows*/


import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4


/*
Container for input items
*/
Rectangle {
  id: rootRectangle

  // Color definitions
  property color tabBarColor: Qt.rgba(236.0/255.0, 236.0/255.0, 236.0/255.0, 1.0)
  property color innerBorderColor: "#AFAFAF"
  property color highlightColor: "#4A90E2"
  property color titleColor: "#4A4A4A"
  property color titleColorHighlighted: "#111111"

  // Tell SplitViewItem (i.e. component wrapper) that settings are available to make it display settings icon.
  property var hasComponentSettings: true

  // Allows to select the available input items
  TabView {
    id: inputTabView

    anchors.fill: parent

    // Position tab bar below the content frame.
    tabPosition: Qt.BottomEdge


    /* Each input item is represented by its corresponding tab inside the input tab bar. */

    Tab {
      title: "Buttons/Arrow Button Input Icon"
      ButtonInput{
        id: buttonI
        anchors.fill: parent
      }
    }


    Tab {
      title: "Buttons/Mouse Input Icon"
      ClickInput{
        id: clickI
        anchors.fill: parent
      }
    }

    Tab {
      title: "Buttons/Text Console Icon"
      TextInputSelf{
        id: textI
        anchors.fill: parent
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
    inputTabView.getTab(inputTabView.currentIndex).item.settingsButtonPressed();
  }
}
