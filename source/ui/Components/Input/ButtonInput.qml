import QtQuick 2.0
import QtQuick.Controls 1.4

Item {

    Rectangle{
        id: middle
        height: 40
        width: 40
        anchors.centerIn: parent
        visible: false
    }

    Button{
        id: up
        width: middle.width
        height: middle.height
        anchors.left: middle.left
        anchors.bottom: middle.top
        Text{
            font.pointSize: 16
            anchors.centerIn: parent
            text: "\u2191"
        }
        onClicked: {
            inputButtonMod.buttonClicked(0);
        }
    }

    Button{
        id: right
        width: middle.width
        height: middle.height
        anchors.left: middle.right
        anchors.bottom: middle.bottom
        Text{
            font.pointSize: 16
            anchors.centerIn: parent
            text: "\u2192"
        }
        onClicked: {
            inputButtonMod.buttonClicked(1);
        }
    }

    Button{
        id: down
        width: middle.width
        height: middle.height
        anchors.left: middle.left
        anchors.top: middle.bottom
        Text{
            font.pointSize: 16
            anchors.centerIn: parent
            text: "\u2193"
        }
        onClicked: {
            inputButtonMod.buttonClicked(2);
        }
    }

    Button{
        id: left
        width: middle.width
        height: middle.height
        anchors.right: middle.left
        anchors.bottom: middle.bottom
        Text{
            font.pointSize: 16
            anchors.centerIn: parent
            text: "\u2190"
        }
        onClicked: {
            inputButtonMod.buttonClicked(3);
        }
    }




}
