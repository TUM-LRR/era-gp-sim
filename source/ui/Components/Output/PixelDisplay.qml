import QtQuick 2.0
import QtQuick.Window 2.0
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
    }

    // Connect the output item to signals that the model might send.
    Connections {
        target: outputComponent
        onNewImage: pixeldisplaypainteditemid.setImage(image)
        // Send when the memory changes (at any address).
        onMemoryChanged: {
            console.log("onMemoryChanged");
            var _baseAddress = outputComponent.getOutputItems()[outputItemIndex]["baseAddress"];
            // Check if the memory address that was changed (at least partly)
            // belongs to the output item's source space.
            if ((address+length) >= _baseAddress && (address <= (_baseAddress+1))) {
                updateContent(_baseAddress);
            }
            //pixeldisplaypainteditemid.paint();
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
        }

        onVisibleChanged: {
            if (visible) {
                updateSettings();
            }
        }
    }


}
