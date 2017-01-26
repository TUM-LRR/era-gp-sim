import QtQuick 2.0
import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Theme 1.0

Item {
  property int outputItemIndex: 2
  property bool currentMode
  property var currentAddress
  id: item

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
      inputConsole.focus = true;
      inputConsole.forceActiveFocus();
      mouse.accepted = false;
    }
    onReleased: mouse.accepted = false;
    onDoubleClicked: mouse.accepted = false;
    onPressAndHold: mouse.accepted = false;
  }

  ScrollView {
    id: scrollView
    anchors.fill: parent

    style: ScrollViewStyle {
      transientScrollBars: true
    }

    Flickable {
      id: flickable
      anchors.fill: parent
      contentHeight: readonlyConsole.height + inputConsole.height
      contentWidth: scrollView.viewport.width

      ////////////////////////////////////////////////////
      // Components for the output part of the console. //
      ///////////////////////////////////////////////////

      TextEdit {
        id: readonlyConsole

        anchors.leftMargin: Theme.console.leftMargin
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        onActiveFocusChanged: {
          if (activeFocus) inputConsole.forceActiveFocus();
        }

        // for correct clipboard behaviour without a clipboard adapter,
        // only output or input can be copied
        onSelectedTextChanged: inputConsole.deselect();

        height: {
          if (text === "") {
            0
          } else {
            contentHeight
          }
        }
        color: Theme.console.textColor
        wrapMode: Text.WrapAnywhere
        text: ""
        readOnly: true
        selectByMouse: true

        //Clears the Screen in pipelike mode
        MouseArea {
          id: clearArea
          anchors.fill:parent
          visible: true
          acceptedButtons: Qt.RightButton
          onDoubleClicked: {
            if (outputComponent.getOutputItem(outputItemIndex)["textMode"] !== 0) {
              readonlyConsole.text = "";
            }
          }
        }

        function clear() {
          text = "";
        }
      }

      //////////////////////////////////////////////////
      // Components for the input part of the console //
      //////////////////////////////////////////////////

      Text {
        id: consolePrompt
        anchors.left: readonlyConsole.left
        anchors.bottom: inputConsole.bottom
        text: Theme.console.prompt
        color: Theme.console.promptColor
      }

      TextEdit {
        id: inputConsole

        selectByMouse: true

        anchors.leftMargin: Theme.console.promptMargin
        anchors.left: consolePrompt.right
        anchors.right: parent.right
        anchors.top: readonlyConsole.bottom

        color: Theme.console.textColor

        Keys.onReturnPressed: {
          var newText = "";
          if (readonlyConsole.text !== "") {
            newText = "\n" + newText;
          }
          newText += inputConsole.text;
          consoleComponent.appendText(newText);
          if (currentMode) {
            consoleComponent.setInterrupt();
          }
          text = "";
          if (y + height >= scrollView.flickableItem.contentY + scrollView.viewport.height) {
            scrollView.flickableItem.contentY = y + height - scrollView.viewport.height;
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

        wrapMode: Text.WrapAnywhere
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
      var baseAddress = parseInt(consoleComponent.getStart());
      var deleteBuffer = consoleComponent.deleteBuffer();
      var textLength = parseInt(consoleComponent.getLength());
      var addressVar = parseInt(address);
      var lengthVar = parseInt(length);

      // Check if the memory address that was changed (at least partly) belongs to
      // the output item's source space.
      if (((addressVar >= baseAddress && addressVar <= (baseAddress + textLength)) ||
         (addressVar + lengthVar >= baseAddress && addressVar <= baseAddress + textLength)) &&
         !currentMode) {
        item.updateContent(baseAddress);
      } else if (consoleComponent.checkInterrupt()) {
        item.updateContent(baseAddress);
        consoleComponent.resetInterrupt();
      }
    }
  }

  Connections {
    target: consoleComponent
    onSettingsChanged: {
      var mode = consoleComponent.deleteBuffer();
      var baseAddress = consoleComponent.getStart();

      if(mode === currentMode && baseAddress === currentAddress) return;

      if (currentMode !== mode) {
        currentMode = mode;
        readonlyConsole.clear();
      }

      if (currentAddress !== baseAddress) {
        currentAddress = baseAddress;
        readonlyConsole.clear();
      }
      item.updateContent(baseAddress);
    }
  }

  function updateContent(baseAddress) {
    if (currentMode) {
      readonlyConsole.text += consoleComponent.getText();
    } else {
      readonlyConsole.text = consoleComponent.getText();
    }
  }

  Component.onCompleted: {
    currentAddress = consoleComponent.getStart();
    currentMode = consoleComponent.deleteBuffer();
    item.updateContent(currentAddress);
  }


  ConsoleSettingsWindow {
    id: settingsWindowConsole
  }

  function settingsButtonPressed() {
    settingsWindowConsole.show();
  }

}
