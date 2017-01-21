import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Theme 1.0

import "../Common"

Rectangle {
    // Color definitions
    property var lightstripInactiveColor: Theme.output.lightStrip.strip.inactive.background
    property var lightstripDefaultActiveColor: Theme.output.lightStrip.strip.active.background

    // Index of output item relative to all available output outems.
    // Required to identify this output item when communicating with
    // the model.
    property var outputItemIndex: 0

    signal settingsButtonPressed()

    color: Theme.output.lightStrip.background

    ListModel {
        id: lightstripModel
    }

    // Update the output item's content (there may already be some initial values in memory).
    Component.onCompleted: {
        updateContent(outputComponent.getOutputItem(outputItemIndex)["baseAddress"]);
    }

    // Connect the output item to signals that the model might send.
    Connections {
        target: outputComponent
        // Send when the memory changes (at any address).
        onMemoryChanged: {
            var _baseAddress = outputComponent.getOutputItem(outputItemIndex)["baseAddress"];
            // Check if the memory address that was changed (at least partly) belongs to
            // the output item's source space.
            if ((address+length) >= _baseAddress && (address <= (_baseAddress+(lightstripModel.count+7)/8))) {
                updateContent(_baseAddress);
            }
        }
        // Send when any item's settings where updated.
        onOutputItemSettingsChanged: {
            updateContent(outputComponent.getOutputItem(outputItemIndex)["baseAddress"]);
            lightstripSettingsWindow.updateSettings();
        }
    }

    // Called from outside by the output tab view to signal that the settings button for the current
    // output item was pressed.
    onSettingsButtonPressed: {
        lightstripSettingsWindow.show();
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
        var numberOfStrips = outputComponent.getOutputItem(outputItemIndex)["numberOfStrips"];
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
                        var _baseAddress = outputComponent.getOutputItem(outputItemIndex)["baseAddress"];
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

    LightstripSettingsWindow {
        id: lightstripSettingsWindow
    }

}
