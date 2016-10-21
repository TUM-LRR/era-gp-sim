import QtQuick 2.0
import QtQuick 2.3
import QtQuick.Controls 1.4

Item {
    Rectangle{
        anchors.fill: parent
        color: "black"

        TextArea{
            id: textarea
            height: parent.height
            width: parent.width
            anchors.top: parent.top
            anchors.left: parent.left

            wrapMode: Text.Wrap
            textColor: "white"

            readOnly: true
            backgroundVisible: false

        }

        Connections{
            target: consoleModel
            onTextChanged:{
                textarea.text=consoleModel.getText();
                console.info(textarea.text);
                console.info("Juhu!");
            }
        }
        Connections{
            target: consoleModel
            onDataChanged: {
                consoleModel.getData();
            }
        }
    }
}
