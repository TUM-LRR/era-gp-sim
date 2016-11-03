import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.0

import "../Common"

Item {
    // Color definitions
    property var lightstripInactiveColor: "#9B9B9B"
    property var lightstripDefaultActiveColor: "#0080FF"

    // Index of output item relative to all available output outems.
    // Required to identify this output item when communicating with
    // the model.
    property var outputItemIndex: 0

    ListModel {
        id: lightstripModel
    }

    // Update the output item's content (there may already be some initial values in memory).
    Component.onCompleted: {
        updateContent(outputComponent.getOutputItems()[outputItemIndex]["baseAddress"]);
    }

    // Connect the output item to signals that the model might send.
    Connections {
        target: outputComponent
        // Send when the memory changes (at any address).
        onMemoryChanged: {
            console.log("onMemoryChanged");
            var _baseAddress = outputComponent.getOutputItems()[outputItemIndex]["baseAddress"];
            // Check if the memory address that was changed (at least partly) belongs to
            // the output item's source space.
            if ((address+length) >= _baseAddress && (address <= (_baseAddress+(lightstripModel.count+7)/8))) {
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

    // Called from outside by the output tab view to signal that the settings button for the current
    // output item was pressed.
    function settingsButtonPressed() {
        settingsWindow.show();
    }

    // Updates the content of the output model depending on the value in memory.
    function updateContent(_baseAddress) {
        _updatelightstripModel();
        var content = outputComponent.getMemoryContent(_baseAddress, (lightstripModel.count+7)/8);

        for (var bitIndex = 0; bitIndex < content.length && bitIndex < lightstripModel.count; ++bitIndex) {
            lightstripModel.setProperty(bitIndex, "active", content[bitIndex]);
        }
    }

    // Updates the lightstripModel to correspond to the output item's settings values.
    function _updatelightstripModel() {
        var numberOfStrips = outputComponent.getOutputItems()[outputItemIndex]["numberOfStrips"];
        if (numberOfStrips < 0) {
            numberOfStrips = 0
        }
        while (lightstripModel.count < numberOfStrips) {
            lightstripModel.append({ activeColor: lightstripDefaultActiveColor, inactiveColor: lightstripInactiveColor, active: false })
        }
        while (lightstripModel.count > numberOfStrips) {
            lightstripModel.remove(lightstripModel.count - 1, 1)
        }
    }

    SystemPalette {
        id: systemColorPalette
        colorGroup: SystemPalette.Active
    }

    // Background/Border of the lightstrip.
    Rectangle{
        id: lightstripBackground
        color: "#00000000"
        anchors.verticalCenter: parent.verticalCenter
        height: parent.height-20
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        radius: 5
        border.color: lightstripInactiveColor
        border.width: 2

        // ListView for lights.
        ListView {
            spacing: (lightstripBackground.width)/(5*lightstripModel.count+1)
            anchors.fill: parent
            anchors.leftMargin: (lightstripBackground.width)/(5*lightstripModel.count+1)
            anchors.rightMargin: (lightstripBackground.width)/(5*lightstripModel.count+1)
            anchors.topMargin: (lightstripBackground.width)/(5*lightstripModel.count+1)
            anchors.bottomMargin: (lightstripBackground.width)/(5*lightstripModel.count+1)

            orientation: ListView.Horizontal
            interactive: false

            model: lightstripModel
            delegate: light
        }
    }

    // Single light.
    Component {
        id: light
        Rectangle{
            id: rect
            property int _index: index

            color: (active == true) ? activeColor : inactiveColor
            height: parent.height
            anchors.verticalCenter: parent.center
            width: (4*lightstripBackground.width)/(5*lightstripModel.count+1)
            border.color: systemColorPalette.highlight
            border.width: 0
            radius: 5

            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    // Show change-color-dialog on right-click.
                    if(mouse.button === Qt.RightButton){
                        var newObject = Qt.createQmlObject('import QtQuick 2.2
                                                            import QtQuick.Dialogs 1.0

                                                            ColorDialog {
                                                                id: colorDialog
                                                                width: 300
                                                                height: 200
                                                                title: "Please choose a color"
                                                                currentColor: rect.color
                                                                onAccepted: {
                                                                    lightstripModel.setProperty(rect._index, "activeColor", colorDialog.color.toString());
                                                                    colorDialog.close();
                                                                }
                                                                onRejected: {
                                                                    colorDialog.close();
                                                                }
                                                                Component.onCompleted: visible = true
                                                            }',
                                                           light,
                                                           "colorChooser");
                    }
                    // Set light active on left-click.
                    else {
                        lightstripModel.setProperty(_index, "active", !lightstripModel.get(_index).active);
                        var memoryContent = [];
                        for (var index = 0; index < lightstripModel.count; ++index) {
                            memoryContent.push(lightstripModel.get(index).active);
                        }
                        var _baseAddress = outputComponent.getOutputItems()[outputItemIndex]["baseAddress"];
                        outputComponent.putMemoryValue(_baseAddress, memoryContent);
                    }
                }
                onEntered: {
                    rect.border.width=1
                }
                onExited: {
                    rect.border.width=0
                }
            }
        }
    }

    // Window for lightstrip settings.
    Window {
        id: settingsWindow
        width: 400
        height: 200

        title: "Lightstrip Settings"

        function updateSettings() {
            numberOfStripsTextField.text = outputComponent.getOutputItems()[outputItemIndex]["numberOfStrips"];
            baseAddressTextField.text = outputComponent.getOutputItems()[outputItemIndex]["baseAddress"];
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
                    text: "Memory Source:"
                }
                Text {
                    text: "Number of Strips:"
                }
            }

            // Settings controls itself.
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

                // Text field for setting the output item's source address in memory.
                TextField {
                    id: baseAddressTextField

                    onAccepted: { processInput(); }
                    onEditingFinished: { processInput(); }

                    function processInput() {
                        var inputValue = controlsColumn.integerFromInputString(String(baseAddressTextField.text))
                        if (inputValue && inputValue > 0) {
                            outputComponent.setOutputItemProperty(outputItemIndex, "baseAddress", inputValue);
                        }
                    }
                }

                TextField {
                    id: numberOfStripsTextField
                    height: baseAddressTextField.height

                    onAccepted: { processInput(); }
                    onEditingFinished: { processInput(); }

                    Component.onCompleted: {
                        settingsWindow.updateSettings();
                    }

                    function processInput() {
                        var inputValue = controlsColumn.integerFromInputString(String(numberOfStripsTextField.text));
                        if (inputValue && inputValue > 0) {
                            outputComponent.setOutputItemProperty(outputItemIndex, "numberOfStrips", inputValue);
                        }
                    }
                }

            }
        }
    }
}
