import QtQuick 2.6
import "../Common"

Rectangle {
    id: helpToolTip

    property alias helpText: helpOverlay.text
    property alias maxWidth: helpOverlay.maxWidth
    property alias maxHeight: helpOverlay.maxHeight
    property alias relativeX: helpOverlay.relativeX
    property alias relativeY: helpOverlay.relativeY

    radius: 3
    color: Qt.rgba(32.0/255.0, 57.0/255.0, 77.0/255.0, 0.85)
    border.color: Qt.rgba(32.0/255.0, 57.0/255.0, 77.0/255.0, 1.0)
    visible: false

    Text {
      anchors.fill: parent
      text: "?"
      color: "white"
      horizontalAlignment: TextEdit.AlignHCenter
      font.bold: true
    }

    function showIcon() {
        visible = true;
        helpOverlay.visible = true;
    }

    function showTooltip() {
      helpOverlay.visible = true;
    }

    function hideIcon() {
        hideTooltip();
        visible = false;
    }

    function hideTooltip() {
        helpOverlay.visible = false;
    }

    // Help overlay
    ToolTip {
        id: helpOverlay
        anchors.fill: parent
        width: parent.width
        height: parent.height
        text: helpText
    }

}
