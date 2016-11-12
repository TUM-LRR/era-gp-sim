import QtQuick 2.0
import QtQuick.Window 2.0

Item {
    property var outputItemIndex: 0

    Rectangle {//Dummy-Module, will be replaced
        id: greenBackground
        width: 360
        height: 360
        // color: "green"
        anchors.fill: parent
    }
    Image {
        id: pixelDisplayImage
        source: "image://pixeldisplayprovider/red"
    }

    // Called from outside by the output tab view to signal that the settings // button for the current output item was pressed.
    function settingsButtonPressed() {
        settingsWindow.show();
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
