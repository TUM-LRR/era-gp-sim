import QtQuick 2.0
import QtQuick.Window 2.0
//import "ImageProviderCore"

Item {
    property var outputItemIndex: 0

    /*Rectangle {//Dummy-Module, will be replaced
        id: greenBackground
        width: 360
        height: 360
        // color: "green"
        anchors.fill: parent
    }*/
    Image {
        //width: 100
        //height: 50
        smooth: false
        id: pixelDisplayImage
        source: "image://pixeldisplayprovider/red"
        anchors.fill: parent
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
