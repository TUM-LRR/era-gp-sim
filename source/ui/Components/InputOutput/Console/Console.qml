/*
*C++ Assembler Interpreter
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
* along with this program. If not, see http://www.gnu.org/licenses/.
*/

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Theme 1.0

Item {
  id: consoleItem

  property bool currentMode
  property var currentAddress

  property alias readonlyConsole: readonlyConsole
  property alias inputConsole: inputConsole

  function clear() {
    consoleComponent.deleteTextInMemory();
    readonlyConsole.text = "";
  }

  ////////////////////////////////////////////////////
  // Components for the output part of the console. //
  ///////////////////////////////////////////////////

  TextEdit {
    id: readonlyConsole

    anchors.leftMargin: Theme.console.promptMargin
    anchors.top: parent.top
    anchors.left: promptColumn.right
    anchors.right: parent.right

    font.family: Theme.console.fontFamily
    font.pixelSize: Theme.console.fontSize
    color: Theme.console.textColor
    wrapMode: Text.WrapAnywhere
    text: ""
    readOnly: true
    selectByMouse: true
    height: {
      if (text === "") {
        return 0;
      } else {
        return contentHeight;
      }
    }

    property int correctedLineCount: {
      if (text === "") {
        return 0;
      } else {
        return lineCount;
      }
    }

    onHeightChanged: {
      if (inputConsole.activeFocus) inputConsole.updateScroll();
    }

    onActiveFocusChanged: {
      if (activeFocus) inputConsole.forceActiveFocus();
    }

    // for correct clipboard behaviour without a clipboard adapter,
    // only output or input can be copied
    onSelectedTextChanged: inputConsole.deselect();
  }

  ////////////////////////////////////
  // Column for the console prompt. //
  ////////////////////////////////////

  Column {
    id: promptColumn
    anchors.left: parent.left
    anchors.top: parent.top
    Repeater {
      model: readonlyConsole.correctedLineCount + 1;
      Text {
        text: Theme.console.prompt
        color: Theme.console.promptColor
        height: inputConsole.cursorRectangle.height
        font: inputConsole.font
      }
    }
  }

  //////////////////////////////////////////////////
  // Components for the input part of the console //
  //////////////////////////////////////////////////

  TextEdit {
    id: inputConsole

    selectByMouse: true

    anchors.leftMargin: Theme.console.promptMargin
    anchors.left: promptColumn.right
    anchors.right: parent.right
    anchors.top: readonlyConsole.bottom

    font.family: Theme.console.fontFamily
    font.pixelSize: Theme.console.fontSize
    color: Theme.console.textColor
    wrapMode: Text.WrapAnywhere

    Keys.onReturnPressed: {
      var newText = "";
      if (readonlyConsole.text !== "") {
        newText = "\n" + newText;
      }
      newText += inputConsole.text;
      // Add a null char as delimiter
      newText += "\0";
      consoleComponent.appendText(newText);
      if (currentMode) {
        consoleComponent.setInterrupt();
      }
      text = "";
    }

    // scroll to cursor, if necessary.
    onCursorRectangleChanged: updateScroll();

    function updateScroll() {
      if (y + height >= scrollView.flickableItem.contentY + scrollView.viewport.height) {
        scrollView.flickableItem.contentY =
        y + height - scrollView.viewport.height + Theme.console.margin*2;
      }
    }

    // for correct clipboard behaviour without a clipboard adapter,
    // only output or input can be copied
    onSelectedTextChanged: readonlyConsole.deselect();

    Keys.onPressed: {
      if (event.matches(StandardKey.Copy)) {
        if (readonlyConsole.selectedText !== "") {
          readonlyConsole.copy();
          event.accepted = true;
        }
      }
    }
  }

  /////////////////////////////////////////////////////////////
  // Connections to update the content according to signals. //
  /////////////////////////////////////////////////////////////

  Connections {
    target: outputComponent
    // Check for changes in the memory (at any address).
    onMemoryChanged: {
      // we have to parse size_t to int, as qml/javascript handles them like strings.
      var baseAddress = parseInt(consoleComponent.getStart());
      var deleteBuffer = consoleComponent.deleteBuffer();
      var textLength = parseInt(consoleComponent.getLength());
      var addressVar = parseInt(address);
      var lengthVar = parseInt(length);

      // Check if the memory address that was changed (at least partly) belongs to
      // the output item's source space.
      var checkBegin = addressVar >= baseAddress &&
          addressVar <= baseAddress + textLength;
      var checkLength = addressVar + lengthVar >= baseAddress &&
          addressVar <= baseAddress + textLength;

      if (!currentMode && (checkBegin || checkLength)) {
        consoleItem.updateContent(baseAddress);
      } else if (consoleComponent.checkInterrupt()) {
        consoleItem.updateContent(baseAddress);
        consoleComponent.resetInterrupt();
      }
    }
  }

  Connections {
    target: consoleComponent
    onSettingsChanged: {
      settingsWindowConsole.updateSettings();
      var mode = consoleComponent.deleteBuffer();
      var baseAddress = consoleComponent.getStart();

      if(mode === currentMode && baseAddress === currentAddress) return;

      currentMode = mode;

      if (currentAddress !== baseAddress) {
        currentAddress = baseAddress;
        consoleItem.clear();
      }

      if (!currentMode) {
        consoleItem.updateContent();
      }
    }
  }

  function updateContent() {
    if (currentMode) {
      readonlyConsole.text += consoleComponent.getText();
    } else {
      readonlyConsole.text = consoleComponent.getText();
    }
  }

  Component.onCompleted: {
    currentAddress = consoleComponent.getStart();
    currentMode = consoleComponent.deleteBuffer();
    consoleItem.updateContent();
  }
}
