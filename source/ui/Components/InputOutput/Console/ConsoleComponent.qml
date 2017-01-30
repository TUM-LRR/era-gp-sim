/* C++ Assembler Interpreter
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
* along with this program. If not, see http://www.gnu.org/licenses/.*/

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Theme 1.0
import "."

Item {
  id: item

  property int outputItemIndex: 2

  Rectangle {
    id: background
    anchors.fill: parent
    color: Theme.console.background
  }

  // MouseArea to get focus for the input text edit.
  MouseArea {
    id: inputFocusMouse
    z: scrollView.z + 1
    anchors.fill: parent
    propagateComposedEvents: false
    acceptedButtons: Qt.LeftButton

    onPressed: {
      consoleText.inputConsole.focus = true;
      consoleText.inputConsole.forceActiveFocus();
      mouse.accepted = false;
    }
    onReleased: mouse.accepted = false;
    onDoubleClicked: mouse.accepted = false;
    onPressAndHold: mouse.accepted = false;
  }

  ContextMenu {}

  ScrollView {
    id: scrollView
    anchors.fill: parent

    style: ScrollViewStyle {
      transientScrollBars: true
    }

    Flickable {
      id: flickable
      anchors.fill: parent
      anchors.margins: Theme.console.margin
      contentHeight: {
        return consoleText.readonlyConsole.height + consoleText.inputConsole.height
      }
      contentWidth: scrollView.viewport.width

      Console { id: consoleText }
    }
  }

  ConsoleSettingsWindow {
    id: settingsWindowConsole
  }

  function settingsButtonPressed() {
    settingsWindowConsole.show();
  }
}
