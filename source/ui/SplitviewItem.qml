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
import "Components"

Item {

  // default value in the item
  property var usual
  property bool isExpanded: false

  onIsExpandedChanged: {
    if (isExpanded == true) {
      headerFadeOut.stop()
      headerFadeIn.start()
    } else {
      headerFadeIn.stop()
      headerFadeOut.start()
    }
  }

  SequentialAnimation {
    id: headerFadeIn
    PauseAnimation { duration: 100 }
    PropertyAnimation { target: header; properties: "height"; to: header.expandedHeight; duration: 100}
    PropertyAnimation { target: headerItems; properties: "visible"; to: true; duration: 0}
  }
  SequentialAnimation {
    id: headerFadeOut
    PauseAnimation { duration: 100 }
    PropertyAnimation { target: headerItems; properties: "visible"; to: false; duration: 0}
    PropertyAnimation { target: header; properties: "height"; to: 8; duration: 100}
  }




  Rectangle {
    id: header
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    height: 8
    color: Qt.rgba(241.0/255.0, 241.0/255.0, 241.0/255.0, 1.0)

    property var expandedHeight: componentSelector.height + 8

    MouseArea {
      anchors.fill: parent
      hoverEnabled: true
      propagateComposedEvents: true

      onHoveredChanged: {
        if (containsMouse) {
          isExpanded=true
        } else {
          isExpanded=false
        }
      }

      Item {
        id: headerItems
        anchors.fill: parent
        visible: false

        // Choose
        ComboBox {
          id: componentSelector

          enabled: true
          anchors.top: parent.top
          anchors.topMargin: 4
          anchors.left: parent.left
          anchors.leftMargin: 4
          anchors.right: (settingsButton.visible === true) ? settingsButton.left : parent.right
          anchors.rightMargin: 4
          model: ["Snapshots", "InputOutput", "Register", "Memory", "Help" ]

          onPressedChanged: {
            if (pressed) {
              isExpanded=true
            } else if (!hovered) {
              isExpanded=false
            }
          }

          property var usualList: ["snapshots", "inputoutput", "register", "memory", "help"]
          onModelChanged: {
            var usualIndex = usualList.indexOf(usual);
            if (usualIndex !== -1) {
              componentSelector.currentIndex = usualList.indexOf(usual);
            } else {
              componentSelector.currentIndex = 0;
            }
          }
        }

        // Button for opening component settings for the current component.
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
                source: (control.pressed) ? "Icons/Settings Icon Pressed.png" : "Icons/Settings Icon.png"
              }
            }
          }
          // Clicking the settings button opens the current components settings menu.
          onClicked: {
            componentLoader.item.settingsButtonPressed();
          }

          visible: (componentLoader.item.hasComponentSettings !== undefined) ? componentLoader.item.hasComponentSettings : false
        }
      }
    }

    Rectangle {
      id: bottomBorder
      color: Qt.rgba(175.0/255.0, 175.0/255.0, 175.0/255.0, 1.0)
      anchors.bottom: parent.bottom
      height: 1
      anchors.left: parent.left
      anchors.right: parent.right
    }

  }

  Loader {
    id: componentLoader

    anchors.top: header.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom

    property var sourceComponents: [
      "Snapshots/SnapshotList.qml",
      "InputOutput/InputOutput.qml",
      "Register/RegisterComponent.qml",
      "Memory/MemoryComponent.qml",
      "help/HelpWindow.qml"
    ]

    source: {
      var currentComponent = sourceComponents[componentSelector.currentIndex];
      if (currentComponent) {
        return  "Components/" + currentComponent;
      } else {
        return "Components/help/HelpWindow.qml";
      }
    }
  }
}
