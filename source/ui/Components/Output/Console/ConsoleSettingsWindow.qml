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
import QtQuick.Layouts 1.3
import "../../../Js/TextUtility.js" as TextUtility
import Theme 1.0

// Window for console settings.
Window {
  id: settingsWindowConsole
  width: Theme.console.settings.width
  height: Theme.console.settings.height
  property int outputItemIndex: 2

  title: "Console Settings"

  // Refreshes the window's control contentItem.
  function updateSettings() {
    baseAddressTextField.text = "0x" + consoleComponent.getStart().toString();
    interruptAddressTextField.text = "0x" + consoleComponent.getInterruptAddress().toString();
    if(consoleComponent.deleteBuffer()) {
      pipeMode.checked = true;
      grid.interruptSettingsOpacity = 1.0;
    } else {
      arrayMode.checked = true;
      grid.interruptSettingsOpacity = 0.0;
    }
  }

  GridLayout {
    id: grid
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    anchors.margins: Theme.console.settings.margin

    columns: 2

    property real interruptSettingsOpacity: 0

    Behavior on interruptSettingsOpacity {
      NumberAnimation {
        duration: 200
        easing.type: Easing.OutExpo
      }
    }

    Text {
      text: "Memory Source Address:"
    }

    // Text field for setting the output item's source address in memory.
    TextField {
      id: baseAddressTextField

      onAccepted: { processInput(); }
      onEditingFinished: { processInput(); }

      // Reads the current input and passes the new value to the model.
      function processInput() {
        var inputValue = TextUtility.convertStringToInteger(String(baseAddressTextField.text));
        var maxSize = outputComponent.getMemorySize();
        if (inputValue !== undefined && inputValue >= 0 && inputValue < maxSize) {
          consoleComponent.setStart(inputValue);
        } else {
          // If an incorrect value was entered, reset to previous value.
          updateSettings();
        }
      }
    }

    Text {
      text: "Interrupt Address:"

      opacity: grid.interruptSettingsOpacity
    }

    TextField {
      id: interruptAddressTextField

      opacity: grid.interruptSettingsOpacity
      onAccepted: processInput();
      onEditingFinished: processInput();

      function processInput() {
        var inputValue = TextUtility.convertStringToInteger(String(baseAddressTextField.text));
        var maxSize = outputComponent.getMemorySize();
        if (inputValue !== undefined && inputValue >= 0 && inputValue < maxSize) {
          consoleComponent.setInterruptAddress(inputValue);
        } else {
          // If an incorrect value was entered, reset to previous value.
          updateSettings();
        }
      }
    }

    ExclusiveGroup { id: modeGroup }

    RadioButton {
      id: arrayMode
      text: "Array Mode"
      exclusiveGroup: modeGroup
      onClicked: {
        consoleComponent.setDeleteBuffer(false);
        grid.interruptSettingsOpacity = 0.0;
      }
    }

    RadioButton {
      id: pipeMode
      text: "Pipe Mode"
      exclusiveGroup: modeGroup
      onClicked: {
        consoleComponent.setDeleteBuffer(true);
        grid.interruptSettingsOpacity = 1.0;
      }
    }

    Component.onCompleted: {
      settingsWindowConsole.updateSettings();
    }
  }


  // Button for accepting setting changes and closing the settings window.
  Button {
    id: doneButton

    text: "Done"

    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.margins: Theme.console.settings.margin

    onClicked: {
      baseAddressTextField.focus = false;
      interruptAddressTextField.focus = false;
      close();
    }
  }
}
