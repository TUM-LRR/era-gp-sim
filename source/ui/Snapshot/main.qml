import QtQuick 2.6
import QtQuick.Window 2.2

Window {
    id: mainWindow
    visible: true

//    MainForm {
//        anchors.fill: parent
//        mouseArea.onClicked: {
//            Qt.quit();
//        }
//    }

    SnapshotList{
        id: list
        objectName: "list"
        anchors.fill: parent
    }






}
