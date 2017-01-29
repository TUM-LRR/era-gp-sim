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

import Theme 1.0

import "InputKeys/"
import "InputMouse/"

Rectangle {
  // Tell SplitViewItem (i.e. component wrapper) that settings are available to make it display settings icon.
  property var hasComponentSettings: true

  // Allows to select the available input items
  TabView {
    id: inputTabView
    anchors.fill: parent

    // Position tab bar below the content frame.
    tabPosition: Qt.BottomEdge

    // Each input item is represented by its
    // corresponding tab inside the input tab bar.

    Tab {
      title: "Buttons/Key Input Icon"
      KeyInput {
        id: keyI
        anchors.fill: parent
      }
    }


    Tab {
      title: "Buttons/Mouse Input Icon"
      ClickInput {
        id: clickI
        anchors.fill: parent
      }
    }

    // Change TabView appearance
    style: TabViewStyle {
      tabBar: Rectangle {
        color: Theme.input.tabBar.background
        // Display border between tab bar and content frame.
        Rectangle {
          height: Theme.input.tabBar.border.width
          anchors.top: parent.top
          anchors.left: parent.left
          anchors.right: parent.right
          color: Theme.input.tabBar.border.color
        }
      }

      tab: Rectangle {
        implicitWidth: icon.width + 20
        implicitHeight: 26
        color: Qt.rgba(0, 0, 0, 0)
        Image {
          id: icon
          anchors.centerIn: parent
          // Tab's title contains prefix for icon file; add
          // suffix depending on selection.
          source: {
            if (styleData.selected) {
              return styleData.title + " Selected.png";
            } else {
              return styleData.title + ".png";
            }
          }
        }
      }
    }
  }

  // Called by SplitViewItem.qml (i.e. component wrapper) when
  // component settings icon was pressed.
  function settingsButtonPressed() {
    inputTabView.getTab(inputTabView.currentIndex).item.settingsButtonPressed();
  }
}
