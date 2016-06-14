import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

Item {
    RowLayout {
        id: menuBar

        //styling
        spacing: 5
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10

        //children
        ComboBox {
            //styling

            currentIndex: 2
            //content
            model: ListModel {
                    id: model
                    ListElement { text: "Binary"; shortform:"BIN"; mask: "xxxxx" }
                    ListElement { text: "Octal"; shortform:"OCT"; mask: "xxxxx" }
                    ListElement { text: "Hexadecimal"; shortform:"HEX"; mask: "xxxx" }
                }
            //functions
            onCurrentIndexChanged:{
                console.log("representation of numbers changed")
            }
        }
        Label {
            id: memory_size
            anchors.right: parent.right
            anchors.rightMargin: 20
            text: "test"
        }
    }
}
