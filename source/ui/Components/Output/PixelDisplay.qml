import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import eragpsim.pixeldisplaypainteditem 1.0

Item {
    property var outputItemIndex: 3

    // Update the output item's content (there may already be some initial
    // values in memory).
    Component.onCompleted: {
        pixeldisplaypainteditemid.memoryChanged(0, 0);
    }

    PixelDisplayPaintedItem{
        id: pixeldisplaypainteditemid
        anchors.fill: parent
        outputComponentPointer : outputComponent
    }

    // Connect the output item to signals that the model might send.
    Connections {
        target: outputComponent
        // Send when the memory changes (at any address).
        onMemoryChanged: {
            pixeldisplaypainteditemid.memoryChanged(address, length);
        }
        // Send when any item's settings where updated.
        onOutputItemSettingsChanged: {
            settingsWindow.updateSettings();
        }
    }

    // Called from outside by the output tab view to signal that the settings // button for the current output item was pressed.
    function settingsButtonPressed() {
        settingsWindow.show();
    }

    // Window for lightstrip settings.
    Window {
        id: settingsWindow
        width: 400
        height: 250
        title: "Pixel Display Settings"

        function updateSettings() {
            // TODO::update in preferred number format
            pixelBaseAddressTextField.text = pixeldisplaypainteditemid.pixelBaseAddress.toString()
            colorBaseAddressTextField.text = pixeldisplaypainteditemid.colorBaseAddress.toString()
            colorModeComboBox.currentIndex = colorModeComboBox.find(pixeldisplaypainteditemid.colorMode)
            widthTextField.text = pixeldisplaypainteditemid.width.toString()
            heightTextField.text = pixeldisplaypainteditemid.height.toString()
        }

        onVisibleChanged: {
            if (visible) {
                updateSettings();
            }
        }
        Row {
            anchors.fill: parent
            anchors.leftMargin: 15
            anchors.topMargin: 15
            anchors.rightMargin: 15
            anchors.bottomMargin: 15

            spacing: 15

            // Title of each settings control.
            Column {
                spacing: 16
                Label {
                    text: "Pixel Base Address:"
                }
                Label {
                    text: "Color Base Address:"
                }
                Label {
                    text: "Color Mode:"
                }
                Label {
                    text: "width:"
                }
                Label {
                    text: "height:"
                }
                Button {
                    id: abortButton
                    text: "abort"
                    onClicked : {settingsWindow.updateSettings();}
                }
            }
            Column {
                id: controlsColumn
                spacing: 8

                function integerFromInputString(input) {
                    var base;
                    if (input.indexOf("0x") == 0) {
                        base = 16;
                        input = input.slice(2);
                    } else if (input.indexOf("0b") == 0) {
                        input = input.slice(2);
                        base = 2;
                    } else {
                      base = 10;
                    }
                    return parseInt(input, base);
                }

                TextField {
                    id: pixelBaseAddressTextField

                    // onAccepted: { processInput(); }
                    // onEditingFinished: { processInput(); }

                    function processInput() {
                        var inputValue = controlsColumn.integerFromInputString(String(pixelBaseAddressTextField.text))
                        if (inputValue >= 0) {
                            pixeldisplaypainteditemid.pixelBaseAddress = inputValue
                        }
                    }
                }

                TextField {
                    id: colorBaseAddressTextField

                    // onAccepted: { processInput(); }
                    // onEditingFinished: { processInput(); }

                    function processInput() {
                        var inputValue = controlsColumn.integerFromInputString(String(colorBaseAddressTextField.text))
                        if (inputValue >= 0) {
                            pixeldisplaypainteditemid.colorBaseAddress = inputValue
                        }
                    }
                }

                ComboBox {
                    id: colorModeComboBox
                    model: [ "RGB", "Monochrome",]
                    // onAccepted: {processInput();}
                    // onActivated: { processInput(); }
                    function processInput() {
                        pixeldisplaypainteditemid.colorMode = colorModeComboBox.currentText
                    }
                }

                TextField {
                    id: widthTextField

                    // onAccepted: { processInput(); }
                    // onEditingFinished: { processInput(); }

                    function processInput() {
                        var inputValue = controlsColumn.integerFromInputString(String(widthTextField.text))
                        if (inputValue && inputValue > 0) {
                            pixeldisplaypainteditemid.width = inputValue
                        }
                    }
                }

                TextField {
                    id: heightTextField

                    // onAccepted: { processInput(); }
                    // onEditingFinished: { processInput(); }

                    function processInput() {
                        var inputValue = controlsColumn.integerFromInputString(String(heightTextField.text))
                        if (inputValue && inputValue > 0) {
                            pixeldisplaypainteditemid.height = inputValue
                        }
                    }
                }

                Button {
                    id: doneButton
                    text: "done"
                    onClicked : {processSettings();}

                    function processSettings() {
                        pixelBaseAddressTextField.processInput();
                        colorBaseAddressTextField.processInput();
                        colorModeComboBox.processInput();
                        widthTextField.processInput();
                        heightTextField.processInput();
                    }
                }
            }
        }
    }
}
