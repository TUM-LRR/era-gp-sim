import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

ApplicationWindow {
    visible: true
    width: 80
    height: 80

    toolBar: ToolbarMainWindow{
        id: toolbar
        height: 50
    }

    Text {
        text: "Hello World"
        anchors.centerIn: parent
    }
}
