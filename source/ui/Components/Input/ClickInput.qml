import QtQuick 2.0



Item {
    Rectangle{
        id: area
        width: 255
        height: 255
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter:  parent.verticalCenter

        color: "blue"

        MouseArea{
            anchors.fill: parent
            onClicked: {
                console.info(mouse.x);
                console.info(mouse.y);
                inputClickMod.newClick(mouse.x, mouse.y);
            }
        }

    }
    Rectangle{
        height: 40
        anchors.bottom: area.top
        width: area.width
        anchors.left: area.left

        Text{
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            text: "0"
        }

        Text {
            font.pointSize: 12
            text: "x\n\u2192" ;
            anchors.centerIn: parent
        }

        Text{
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            text: "255"
        }
    }

    Rectangle{
        width: 40
        height: area.height
        anchors.right: area.left
        anchors.top: area.top
        Text{
            anchors.top: parent.top
            anchors.right: parent.right
            text: "0  "
        }

        Text {
            font.pointSize: 12
            anchors.centerIn: parent
            text: "y \u2193"
        }

        Text{
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            text: "255  "
        }
    }

}
