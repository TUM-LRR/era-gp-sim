import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

Item {
    property alias memory_value: memory_size_chooser.value
    RowLayout {
        id: menuBar

        //styling
        spacing: 5
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.topMargin: 2
        anchors.bottomMargin: 2

        //children
        ComboBox {
            //content
            Layout.alignment: Qt.AlignLeft
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
        NumericUpDown{
            id: memory_size_chooser
            anchors.left: parent.left
            anchors.leftMargin: 150
        }
        Label {
            id: memory_size
            Layout.alignment: Qt.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            text: memory_size_chooser.value.toString()
        }
    }
}
