import QtQuick 2.6
import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Item {
    property int outputItemIndex: 2
    property int oldMode: 0
    property var oldAddress: 0
    id: item

    TextArea {
        id: textarea
        anchors.fill: parent
        style: TextAreaStyle {
            textColor: "white"
            backgroundColor: "black"
            renderType: Text.QtRendering
        }
        wrapMode: Text.Wrap
        readOnly: true
        backgroundVisible: true

        //Clears the Screen in pipelike mode
        MouseArea {
            id: clearArea
            anchors.fill:parent
            visible: true
            acceptedButtons: Qt.RightButton
            onDoubleClicked: {
                if (outputComponent.getOutputItem(outputItemIndex)["textMode"] !== 0) {
                    textarea.clear();
                }
            }
        }
        function clear() {
            text = "";
        }
    }

    Connections {
        target: outputComponent
        // Send when the memory changes (at any address).
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
            textarea.clear();
        }

        if (oldAddress !== _baseAddress) {
            oldAddress = _baseAddress;
            textarea.clear();
        }

        var currentText = textarea.text;
        textarea.text = outputComponent.getTextFromMemory(_baseAddress, currentText, mode);
    }

    Component.onCompleted: {
        it.updateContent(outputComponent.getOutputItem(outputItemIndex)["baseAddress"]);
    }


    ConsoleSettingsWindow {
        id: settingsWindowC
    }

    function settingsButtonPressed() {
        settingsWindowC.show();
    }

}
