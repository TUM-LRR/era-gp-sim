import QtQuick 2.6
import QtQuick.Controls 1.5


ApplicationWindow {
    visible: true
    width: 640
    height: 480
    minimumHeight: 50
    minimumWidth: 50
    title: qsTr("Hello World")



    MemoryComponent{
        anchors.fill: parent
    }

}
