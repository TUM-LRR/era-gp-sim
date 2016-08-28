import QtQuick 2.6

//the size of this item is the size of the tooltip trigger area
Item {
    id: toolTipItem

    property alias text: toolTipText.text
    property alias textColor: toolTipText.color
    property alias font: toolTipText.font
    property alias fontPointSize: toolTipText.font.pointSize
    property alias fontPixelSize: toolTipText.font.pixelSize
    property alias textFormat: toolTipText.textFormat

    //delay for triggering the animations
    property real showDelay: 200
    property real hideDelay: 500

    //duration of the animations
    property real showDuration: 250
    property real hideDuration: 200

    property color color: "lightgray"
    property color borderColor: "steelblue"

    property bool moveTooltipWithMouse: false;

    //MouseArea for showing the tooltip, includes the area of the parent object/specified height and length of the tooltip trigger and the tooltip itself
    MouseArea{
        id: errorArea
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        acceptedButtons: Qt.LeftButton

        onEntered: {
            if(!toolTipRect.visible) {
                toolTipRect.show();
                toolTipRect.x = mouseX + 10;
            }
            else if(toolTipRect.visible && (hideAnimation.running || hideWithoutPause.running)){
                toolTipRect.show();
            }
        }

        onClicked: {
            mouse.accepted = false;
        }

        onPressed: {
            mouse.accepted = false;
        }

        onReleased: mouse.accepted = false;
        onDoubleClicked: mouse.accepted = false;
        onPressAndHold: mouse.accepted = false;

        onExited: {
            if(!toolTipArea.focus){
                toolTipRect.hide();
            }
        }

        onPositionChanged: {
            if(moveTooltipWithMouse){
                toolTipRect.x = mouseX + 10
                toolTipRect.y = mouseY - 10
            }
            mouse.accepted = false;
        }

        //rectangle around the tooltip text
        Rectangle{
            id: toolTipRect
            y: toolTipItem.height
            property color transparentColor: Qt.rgba(toolTipItem.color.r, toolTipItem.color.g, toolTipItem.color.b, 0.2)
            property color activeColor: toolTipItem.color
            property real widthMax: toolTipText.contentWidth + toolTipText.leftPadding*2
            property real heightMax: toolTipText.contentHeight + toolTipText.topPadding*2
            width: widthMax
            height: heightMax
            color: transparentColor
            border.color: Qt.rgba(toolTipItem.borderColor.r, toolTipItem.borderColor.g, toolTipItem.borderColor.b, 0.2)
            visible: false


            //show
            function show() {
                if(!hideAnimation.running && !hideWithoutPause.running) {
                    toolTipRect.height = 0;
                    toolTipText.height = 0;
                }
                if(hideAnimation.running || hideWithoutPause.running) {
                    hideAnimation.stop();
                    hideWithoutPause.stop();
                    toolTipRect.visible = true;
                    showWithoutPause.start();
                }
                else{
                    toolTipRect.visible = true;
                    showAnimation.start();
                }
            }

            SequentialAnimation {
                id: showAnimation

                PauseAnimation {
                    duration: showDelay
                }

                ParallelAnimation {
                    NumberAnimation {
                        target: toolTipRect
                        property: "height"
                        duration: showDuration
                        to: toolTipRect.heightMax
                    }

                    NumberAnimation {
                        target: toolTipText
                        property: "height"
                        to: toolTipText.contentHeight + toolTipText.topPadding
                        duration: showDuration
                    }
                }
            }

            ParallelAnimation {
                id: showWithoutPause
                NumberAnimation {
                    target: toolTipRect
                    property: "height"
                    duration: showDuration
                    to: toolTipRect.heightMax
                }

                NumberAnimation {
                    target: toolTipText
                    property: "height"
                    to: toolTipText.contentHeight + toolTipText.topPadding
                    duration: showDuration
                }
            }

            //hide
            function hide() {
                if(showAnimation.running || showWithoutPause.running){
                    showAnimation.stop();
                    showWithoutPause.stop();
                    hideWithoutPause.start();
                }
                else {
                    hideAnimation.start();
                }
            }

            SequentialAnimation {
                id: hideAnimation

                PauseAnimation {
                    duration: hideDelay
                }

                ParallelAnimation {
                    NumberAnimation {
                        target: toolTipRect
                        property: "height"
                        to: 0.0
                        duration: hideDuration
                    }

                    NumberAnimation {
                        target: toolTipText
                        property: "height"
                        to: 0.0
                        duration: hideDuration
                    }
                }
                onStopped: {
                    toolTipRect.visible = false;
                }
            }

            ParallelAnimation {
                id: hideWithoutPause
                NumberAnimation {
                    target: toolTipRect
                    property: "height"
                    to: 0.0
                    duration: hideDuration
                }

                NumberAnimation {
                    target: toolTipText
                    property: "height"
                    to: 0.0
                    duration: hideDuration
                }
                onStopped: {
                    toolTipRect.visible = false;
                }
            }

            //text of the tooltip
            Text {
                id: toolTipText
                width: toolTipItem.width/2
                clip: true
                wrapMode: Text.WrapAnywhere
                textFormat: Text.StyledText
                leftPadding: font.pixelSize/4
                topPadding: font.pixelSize/4
            }
        }

        //MouseArea the same size as the tooltip itself, for focusing on the tooltip
        MouseArea {
            id: toolTipArea
            x: toolTipRect.x
            y: toolTipRect.y
            width: toolTipRect.width
            height: toolTipRect.height
            visible: toolTipRect.visible
            hoverEnabled: true
            onFocusChanged: {
                if(focus) {
                    toolTipRect.color = toolTipRect.activeColor;
                }
                if(!focus) {
                    toolTipRect.color = toolTipRect.transparentColor;
                    if(!errorArea.containsMouse) {
                        toolTipRect.hide();
                    }
                }
            }
            onClicked: {
                focus = true;
            }
        }
    }
}
