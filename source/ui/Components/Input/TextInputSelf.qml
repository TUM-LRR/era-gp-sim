import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Item {
    Rectangle{
        color: "black"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        height: 30

        anchors.verticalCenter: parent.verticalCenter

        TextField{
            id: text
            anchors.fill: parent

            readOnly: false
            maximumLength: 20

            menu: null

            textColor: "green"
            font.pointSize: 12
            font.bold: true

            style: TextFieldStyle{
                background: Rectangle{
                color: "black"
                }
            }

            onAccepted: {
                inputtextMod.newText(text.text);
            }


        }

    }
}
