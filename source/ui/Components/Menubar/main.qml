import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

ApplicationWindow {
    visible: true
    height: 80
    width: 90
    menuBar: Menubar{
        id: menu
    }

    toolBar: ToolbarMainWindow{
        id: tool
    }

    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }
}
