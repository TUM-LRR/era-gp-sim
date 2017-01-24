import QtQuick 2.0
import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Theme 1.0

Item {
  property int outputItemIndex: 2
  property int oldMode: 0
  property var oldAddress: 0
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
        //anchors.bottomMargin: inputConsole.cursorRectangle.height/8
        text: "$ "
        color: Theme.console.promptColor
      }

      TextEdit {
        id: inputConsole

        selectByMouse: true

        anchors.left: consolePrompt.right
        anchors.right: parent.right
        anchors.top: readonlyConsole.bottom

        color: Theme.console.textColor

        Keys.onReturnPressed: {
          var newText = readonlyConsole.text;
          if(newText !== "") {
            newText += "\n";
          }
          newText += inputConsole.text;
          consoleComponent.newText(newText);
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

  Connections {
    target: outputComponent
    // Check for changes in the memory (at any address).
    onMemoryChanged: {
      var _baseAddress = outputComponent.getOutputItem(outputItemIndex)["baseAddress"];
      var _mode = outputComponent.getOutputItem(outputItemIndex)["textMode"];
      // Check if the memory address that was changed (at least partly) belongs to
      // the output item's source space.
      if ((address + length) >= _baseAddress && _mode === 0) {
        item.updateContent(_baseAddress);
      } else if (_mode !== 0 ){
        if(address <= _baseAddress && address + length >= _baseAddress) {
          item.updateContent(_baseAddress);
        }
      }
    }
    // Send when any item's settings where updated.
    onOutputItemSettingsChanged: {
      var mode = outputComponent.getOutputItem(outputItemIndex)["textMode"];
      var baseAddress = outputComponent.getOutputItem(outputItemIndex)["baseAddress"];
      if(mode === oldMode && baseAddress === oldAddress) {
        return;
      }
      item.updateContent(outputComponent.getOutputItem(outputItemIndex)["baseAddress"]);
      settingsWindowC.updateSettings();
    }
  }

  function updateContent(_baseAddress) {
    var mode = outputComponent.getOutputItem(outputItemIndex)["textMode"];
    if (oldMode !== mode) {
      oldMode = mode;
      readonlyConsole.clear();
    }

    if (oldAddress !== _baseAddress) {
      oldAddress = _baseAddress;
      readonlyConsole.clear();
    }

    var currentText = readonlyConsole.text;
    readonlyConsole.text = outputComponent.getTextFromMemory(_baseAddress, currentText, mode);
  }

  Component.onCompleted: {
    item.updateContent(outputComponent.getOutputItem(outputItemIndex)["baseAddress"]);
  }


  ConsoleSettingsWindow {
    id: settingsWindowC
  }

  function settingsButtonPressed() {
    settingsWindowC.show();
  }

}
