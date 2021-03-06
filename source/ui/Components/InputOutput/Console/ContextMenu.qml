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
import QtQuick.Controls 1.5

Item {
  anchors.fill: parent

  Menu {
    id: menu
    MenuItem {
      text: "Clear"
      visible: true
      onTriggered: {
        consoleText.clear();
      }
    }

    MenuItem {
      text: "Switch to Array Mode"
      visible: consoleText.currentMode;
      onTriggered: {
        consoleComponent.setDeleteBuffer(false);
      }
    }

    MenuItem {
      text: "Switch to Pipe Mode"
      visible: !consoleText.currentMode;
      onTriggered: {
        consoleComponent.setDeleteBuffer(true);
      }
    }
  }

  MouseArea {
    anchors.fill: parent
    acceptedButtons: Qt.RightButton
    onClicked: {
      menu.popup();
    }
  }
}
