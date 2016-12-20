import QtQuick 2.6
import "../Common"

ToolTip {
    id: helpToolTip

    property string helpText: ""

    text: "?"
    cornerRadius: 3
    relativeY: 0
    backgroundColor: Qt.rgba(32.0/255.0, 57.0/255.0, 77.0/255.0, 0.85)
    borderColor: Qt.rgba(32.0/255.0, 57.0/255.0, 77.0/255.0, 1.0)
    textColor: "white"
    horizontalTextAlign: TextEdit.AlignHCenter
    font.bold: true
    showDuration: 400
    showDelay: 500
    backgroundColorOnHovered: Qt.rgba(2.0/255.0, 27.0/255.0, 47.0/255.0, 0.85)

    // The tool tip should show right after it has been created. The tool tip appearing
    // when the cursor is position inside a keyword is the desired behavior. However,
    // as the tool tip is created just after the click, it's not able to perceive it's signal.
    // Therefore, it generally appears after creation.
    Component.onCompleted: {
        helpToolTip.showToolTip();
    }

    // Display help overlay when help tool tip was clicked.
    toolTipClickHandler: function() {
        helpOverlayLoader.sourceComponent = helpOverlay;
    }

    // Displays the help overlay, when the help tool tip is clicked.
    Loader {
        id: helpOverlayLoader
        anchors.top: parent.top
        anchors.topMargin: parent.height
        anchors.left: parent.left
    }

    // Help overlay component
    Component {
        id: helpOverlay
        Rectangle {
            width: helpTextLabel.contentWidth+10
            height: helpTextLabel.contentHeight+10
            color: "#EFEFEF"
            border.color: "#DDDDDD"
            radius: 3

            TextEdit {
                id: helpTextLabel
                text: helpText

                width: contentWidth
                height: contentHeight
                anchors.centerIn: parent

                readOnly: true
                activeFocusOnPress: false
                clip: true
                wrapMode: TextEdit.NoWrap
                textFormat: Text.RichText
            }
        }
    }

    // Underline
    Rectangle {
        id: horizontalLine
        color: "#9B9B9B"

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: 1
    }
    Rectangle {
        id: verticalLineLeft
        color: "#9B9B9B"

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: 1
        height: 3
    }
    Rectangle {
        id: verticalLineRight
        color: "#9B9B9B"

        anchors.bottom: parent.bottom
        anchors.left: parent.right
        width: 1
        height: 3
    }

}
