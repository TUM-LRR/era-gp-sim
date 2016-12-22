import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import eragpsim.pixeldisplaypainteditem 1.0

Item {
    property var outputItemIndex: 0

    /*Rectangle {//Dummy-Module, will be replaced
        id: greenBackground
        width: 360
        height: 360
        // color: "green"
        anchors.fill: parent
    }*/
    /*Image {
        //width: 100
        //height: 50
        smooth: false
        id: pixelDisplayImage
        source: "image://pixeldisplayprovider/red"
        anchors.fill: parent
    }*/

    // Update the output item's content (there may already be some initial
    // values in memory).
    Component.onCompleted: {
        updateContent(outputComponent.getOutputItems()[outputItemIndex]["baseAddress"]);
    }
    PixelDisplayPaintedItem{
        id: pixeldisplaypainteditemid
        anchors.fill: parent
        outputComponentPointer : outputComponent
    }

    // Connect the output item to signals that the model might send.
    Connections {
        target: outputComponent
        onNewImage: pixeldisplaypainteditemid.setImage(image)
        // Send when the memory changes (at any address).
        onMemoryChanged: {
            console.log("onMemoryChanged");
            var _baseAddress = outputComponent.getOutputItems()[outputItemIndex]["baseAddress"];
            pixeldisplaypainteditemid.memoryChanged(address, length);
            // Check if the memory address that was changed (at least partly)
            // belongs to the output item's source space.
            if ((address+length) >= _baseAddress && (address <= (_baseAddress+1))) {
                updateContent(_baseAddress);
            }
        }
        // Send when any item's settings where updated.
        onOutputItemSettingsChanged: {
            console.log("onOutputItemSettingsChanged");
            updateContent(outputComponent.getOutputItems()[outputItemIndex]["baseAddress"]);
            settingsWindow.updateSettings();
        }
    }

    // Called from outside by the output tab view to signal that the settings // button for the current output item was pressed.
    function settingsButtonPressed() {
        settingsWindow.show();
    }

    // Updates the content of the output model depending on the value in memory.
    function updateContent(_baseAddress) {
        //_updatelightstripModel();
        var content = outputComponent.getMemoryContent(_baseAddress, 1);
        console.log(content);

        //for (var bitIndex = 0; bitIndex < content.length && bitIndex < lightstripModel.count; ++bitIndex) {
        //    lightstripModel.setProperty(bitIndex, "active", content[bitIndex]);
        //}
    }

    // Window for lightstrip settings.
    Window {
        id: settingsWindow
        width: 400
        height: 200
        title: "Pixel Display Settings"

        function updateSettings() {
          // textField.text = property
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
                Text {
                    text: "Pixel Base Address:"
                }
                Text {
                    text: "Color Base Address:"
                }
                Text {
                    text: "Color Mode:"
                }
                Text {
                    text: "width:"
                }
                Text {
                    text: "height:"
                }
            }
            Column {
                id: controlsColumn
                spacing: 8

                function integerFromInputString(input) {
                    var base = 10;
                    if (input.indexOf("0x") == 0) {
                        base = 16;
                        input = input.slice(2);
                    } else if (input.indexOf("0b") == 0) {
                        input = input.slice(2);
                        base = 2;
                    }
                    return parseInt(input, base);
                }

                TextField {
                    id: pixelBaseAddressTextField

                    onAccepted: { processInput(); }
                    onEditingFinished: { processInput(); }

                    function processInput() {
                        var inputValue = controlsColumn.integerFromInputString(String(pixelBaseAddressTextField.text))
                        if (inputValue >= 0) {
                            pixeldisplaypainteditemid.pixelBaseAddress = inputValue
                        }
                    }
                }

                TextField {
                    id: colorBaseAddressTextField

                    onAccepted: { processInput(); }
                    onEditingFinished: { processInput(); }

                    function processInput() {
                        var inputValue = controlsColumn.integerFromInputString(String(colorBaseAddressTextField.text))
                        if (inputValue >= 0) {
                            pixeldisplaypainteditemid.colorBaseAddress = inputValue
                        }
                    }
                }

                ComboBox {
                    id: colorModeComboBox
                    //width: 200
                    model: [ "Monochrome", "RGB" ]
                    onAccepted: {
                        pixeldisplaypainteditemid.colorMode = colorModeComboBox.currentIndex
                    }
                    onActivated: {
                        pixeldisplaypainteditemid.colorMode = colorModeComboBox.currentIndex
                    }
                }

                // TextField {
                //     id: colorModeTextField
                //
                //     onAccepted: { processInput(); }
                //     onEditingFinished: { processInput(); }
                //
                //     function processInput() {
                //         var inputValue = controlsColumn.integerFromInputString(String(colorModeTextField.text))
                //         if (inputValue && inputValue >= 0) {
                //             pixeldisplaypainteditemid.colorMode = inputValue
                //         }
                //     }
                // }

                TextField {
                    id: widthTextField

                    onAccepted: { processInput(); }
                    onEditingFinished: { processInput(); }

                    function processInput() {
                        var inputValue = controlsColumn.integerFromInputString(String(widthTextField.text))
                        if (inputValue && inputValue > 0) {
                            console.log("width changed")
                            pixeldisplaypainteditemid.width = inputValue
                        }
                    }
                }

                TextField {
                    id: heightTextField

                    onAccepted: { processInput(); }
                    onEditingFinished: { processInput(); }

                    function processInput() {
                        var inputValue = controlsColumn.integerFromInputString(String(heightTextField.text))
                        if (inputValue && inputValue > 0) {
                            pixeldisplaypainteditemid.height = inputValue
                        }
                    }
                }

            }
        }
    }


}
