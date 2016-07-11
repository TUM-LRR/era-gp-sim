import QtQuick 2.0

Item {
    anchors.fill: parent

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            rec.visible=true
        }
    }

    Rectangle{
        id: rec
        anchors.fill: parent
        color: "red"
        visible: false
    }
}
