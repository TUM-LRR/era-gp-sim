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
import Theme 1.0

TabView {
  id: tabView
  anchors.fill: parent

  Component.onCompleted: createProject()

  onCurrentIndexChanged: updateMenuState();

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
    property color tabColorActive: Theme.tabbar.tab.active.background
    property color tabColorInactive: Theme.tabbar.tab.inactive.background
    property color tabBarBackgroundColor: Theme.tabbar.background
    property color componentBackgroundColor: Theme.tabbar.frame.background
    property color tabTitleColor: Theme.tabbar.tab.color
    property color tabTitleColorHighlighted: Theme.tabbar.tab.highlighted.color
    property color tabCloseButtonColor: Theme.tabbar.tab.closeButton.color
    property color tabCloseButtonColorHighlighted: Theme.tabbar.tab.closeButton.highlighted.color
    property color addTabButtonBackgroundColor: Theme.tabbar.addTabButton.background
    property color addTabButtonColorHighlighted: Theme.tabbar.addTabButton.highlighted.color
    property color addTabButtonColor: Theme.tabbar.addTabButton.color

    property int tabHeight: Theme.tabbar.tab.height

    // Overlap between tabs and the components below set to leave a small space
    // which gives the impression of stacked record cards.
    frameOverlap: -3

    // Sets the style of the blank area next to the tabs
    tabBar: Rectangle {
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
        MouseArea {
          acceptedButtons: Qt.NoButton
          anchors.fill: parent
          cursorShape: Qt.PointingHandCursor
        }
        style: ButtonStyle {
          background: Rectangle {
            color: addTabButtonBackgroundColor
          }
          label: Text {
            text: "\u002B"  // + unicode
            anchors.centerIn: parent
            color: control.pressed ? addTabButtonColorHighlighted : addTabButtonColor
            font.bold: true
            font.pointSize: Theme.tabbar.addTabButton.pointSize
          }
        }
        onClicked: createProject();
      }
    }

    // Custom style for tabs
    tab: Rectangle {
      id: tabStyle
      color: styleData.selected ? tabColorActive : tabColorInactive
      implicitWidth: Math.max(tabTitle.width + tabCloseButton.width + 8, Theme.tabbar.tab.minWidth)
      implicitHeight: tabHeight

      MouseArea {
        acceptedButtons: Qt.NoButton
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
      }

      // Add fake (i.e. actually inside the tab not inside the tab bar) top spacing between upper tab edge and tab bar.
      Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 2
        color: tabBarBackgroundColor
      }
      // Container for centering tabTitle between left tab edge and tabCloseButton.
      Item {
        anchors.left: parent.left
        anchors.right: tabCloseButton.left
        anchors.rightMargin: -6 // Correction term to find the actual left edge of character (not text component)
        anchors.verticalCenter: parent.verticalCenter
        width: tabTitle.width + tabCloseButton.width
        Text {
          id: tabTitle
          anchors.centerIn: parent
          text: styleData.title
          color: tabTitleColor
        }
      }
      Button {
        id: tabCloseButton
        property var buttonHeight: 0.65*tabHeight
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 4
        height: buttonHeight
        width: buttonHeight
        MouseArea {
          acceptedButtons: Qt.NoButton
          anchors.fill: parent
          cursorShape: Qt.PointingHandCursor
        }
        style: ButtonStyle {
          background: Rectangle {
            implicitWidth: tabCloseButton.buttonHeight
            implicitHeight: tabCloseButton.buttonHeight
            color: "transparent"
            Text {
              text: "\u00d7"  // Cross or x unicode
              anchors.centerIn: parent
              color: {
                if (control.hovered) {
                  return tabCloseButtonColorHighlighted;
                } else {
                  return tabCloseButtonColor;
                }
              }
              font.bold: true
            }
          }
        }
        onClicked: closeProject();
      }
    }
  }
}
