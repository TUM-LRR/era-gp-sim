import QtQuick 2.6
import QtQuick.Window 2.2

Window {
    visible: true

    ToolbarMainWindow{
        id: toolbar
        height: 50
    }

    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }
}
