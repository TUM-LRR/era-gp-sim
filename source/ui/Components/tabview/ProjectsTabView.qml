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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TabView {
  id: tabView
  anchors.fill: parent

  Component.onCompleted: {
    createProject()
  }

  onCurrentIndexChanged: {
    updateMenuState();
  }

  // get the project of the current tab, is undefined if there is no tab.
  function getCurrentProjectItem() {
    return tabView.getTab(tabView.currentIndex).item;
  }

  // get the id of the project in the current tab, undefined if there is no tab.
  function getCurrentProjectId() {
    return getCurrentProjectItem().projectId;
  }

  // returns false if there is only a creation screen in the current tab.
  // Undefined if there is no tab.
  function currentProjectIsValid() {
    return getCurrentProjectItem().projectValid;
  }

  style: TabViewStyle {
    property color tabColorActive: "white"
    property color tabColorInactive: Qt.rgba(212.0/255.0, 212.0/255.0, 212.0/255.0, 1.0)
    property color tabBarBackgroundColor: Qt.rgba(236.0/255.0, 236.0/255.0, 236.0/255.0, 1.0)
    property color componentBackgroundColor: Qt.rgba(242.0/255.0, 242.0/255.0, 242.0/255.0, 1.0)
    property color tabTitleColor: "#666666"
    property color tabTitleColorHighlighted: "#444444"
    property color tabCloseButtonColor: "#666666"
    property color tabCloseButtonColorHighlighted: "#666666"

    property int tabHeight: 25

    // Overlap between tabs and the components below set to leave a small space
    // which gives the impression of stacked record cards.
    frameOverlap: -3

    // Sets the style of the blank area next to the tabs
    tabBar : Rectangle {
      color: tabBarBackgroundColor
    }

    // Sets the style of the background of an open tab window
    frame : Rectangle {
      color: componentBackgroundColor
    }

    // Adds a plus button to the left corner
    leftCorner: Rectangle {
      implicitWidth: tabHeight
      implicitHeight: tabHeight
      color: tabBarBackgroundColor
      Button {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -2
        style: ButtonStyle {
          background: Rectangle {
            color: "transparent"
          }
          label: Text {
            text: "\u002B"  // + unicode
            anchors.centerIn: parent
            color: control.pressed ? tabCloseButtonColorHighlighted : tabCloseButtonColor
            font.bold: true
            font.pointSize: 18
          }
        }
        onClicked: {
          createProject();
        }
      }
    }

    // Custom style for tabs
    tab: Rectangle {
      id: tabStyle
      color: styleData.selected ? tabColorActive : tabColorInactive
      implicitWidth: Math.max(tabTitle.width + tabCloseButton.width + 8, 80)
      implicitHeight: tabHeight

      // Add fake (i.e. actually inside the tab not inside the tab bar) top spacing between upper tab edge and tab bar.
      Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 2
        color: tabBarBackgroundColor
      }

      Item {
        anchors.centerIn: parent
        width: tabTitle.width + tabCloseButton.width
        // Tab title
        Text {
          id: tabTitle
          anchors.left: parent.left
          anchors.verticalCenter: parent.verticalCenter
          text: styleData.title
          color: tabTitleColor
        }
        // Tab close Button
        Button {
          id: tabCloseButton
          property var buttonHeight: 0.65*tabHeight
          anchors.left: tabTitle.right
          anchors.verticalCenter: tabTitle.verticalCenter
          height: buttonHeight
          width: buttonHeight
          style: ButtonStyle {
            background: Rectangle {
              implicitWidth: tabCloseButton.buttonHeight
              implicitHeight: tabCloseButton.buttonHeight
              color: "transparent"
              Text {
                text: "\u00d7"  // Cross or x unicode
                anchors.centerIn: parent
                color: control.hovered ? tabTitleColorHighlighted : tabTitleColor
                font.bold: true
              }
            }
          }
          onClicked: {
            closeProject();
          }
        }
      }
    }
  }
}
