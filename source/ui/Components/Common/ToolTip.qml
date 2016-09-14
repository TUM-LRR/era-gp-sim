import QtQuick 2.6

// The size of this item is the size of the tooltip trigger area
Item {
    id: toolTipItem

    // Text properties
    property alias text: toolTipText.text
    property alias textColor: toolTipText.color
    property alias font: toolTipText.font
    property alias fontPointSize: toolTipText.font.pointSize
    property alias fontPixelSize: toolTipText.font.pixelSize
    property alias textFormat: toolTipText.textFormat

    // Background properties
    property color backgroundColor: Qt.rgba(239.0/255.0, 239.0/255.0, 239.0/255.0, 1.0);
    property color backgroundColorOnFocus: backgroundColor
    property color borderColor: "#666666"
    property color borderColorOnFocus: "#444444"
    property real cornerRadius: 0

    // Delay for triggering the animations
    property real showDelay: 200
    property real hideDelay: 500

    // Duration of the animations
    property real showDuration: 250
    property real hideDuration: 200

    property bool moveTooltipWithMouse: false;

    // MouseArea for showing the tooltip, includes the area of the parent object/specified height and length of the tooltip trigger and the tooltip itself
    MouseArea{
        id: toolTipHitArea
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        acceptedButtons: Qt.LeftButton

        // Pass through un-handled mouse events.
        onClicked: mouse.accepted = false;
        onPressed: mouse.accepted = false;
        onReleased: mouse.accepted = false;
        onDoubleClicked: mouse.accepted = false;
        onPressAndHold: mouse.accepted = false;

        onEntered: {
            if(!toolTipRect.visible) {
                toolTipRect.show();
                toolTipRect.x = mouseX + 10;
            }
            else if(toolTipRect.visible && (hideAnimation.running || hideWithoutPause.running)){
                toolTipRect.show();
            }
        }

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

        // Rectangle around the tooltip text
        Rectangle{
            id: toolTipRect

            // The width and height of the tooltip's background depends on the tooltip's text.
            property real _widthMax: toolTipText.contentWidth + toolTipText.leftPadding*2
            property real _heightMax: toolTipText.contentHeight + toolTipText.topPadding*2

            y: toolTipItem.height
            width: _widthMax
            height: _heightMax
            color: backgroundColor
            border.color: Qt.rgba(toolTipItem.borderColor.r, toolTipItem.borderColor.g, toolTipItem.borderColor.b, 0.2)
            visible: false
            opacity: 0.0
            radius: cornerRadius

            // Text of the tooltip
            TextEdit {
                id: toolTipText
                width: 300
                height: toolTipText.contentHeight + toolTipText.topPadding
                clip: true
                wrapMode: Text.WrapAnywhere
                textFormat: Text.StyledText
                leftPadding: font.pixelSize/4
                topPadding: font.pixelSize/4
            }

            // Showing the tooltip
            function show() {
                // Make sure the property which is about to be animated is set to
                // a proper initial value, if it's not currently in the process of
                // showing an animation.
                if(!hideAnimation.running && !hideWithoutPause.running) {
                    toolTipRect.opacity = 0.0;
                }

                // If the tooltip is in the process of hiding and is hovered again, it's
                // supposed to show instantly.
                if(hideAnimation.running || hideWithoutPause.running) {
                    hideAnimation.stop();
                    hideWithoutPause.stop();
                    toolTipRect.visible = true;
                    showWithoutPause.start();
                }
                // Otherwise, animate.
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
                        property: "opacity"
                        to: 1.0
                        duration: showDuration
                    }
                }
            }

            ParallelAnimation {
                id: showWithoutPause
                NumberAnimation {
                    target: toolTipRect
                    property: "opacity"
                    to: 1.0
                    duration: showDuration
                }
            }

            // Hiding the tooltip
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
                        property: "opacity"
                        to: 0.0
                        duration: hideuration
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
                    property: "opacity"
                    to: 0.0
                    duration: hideDuration
                }
                onStopped: {
                    toolTipRect.visible = false;
                }
            }
        }


        // MouseArea the same size as the tooltip itself, for focusing on the tooltip.
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
                    toolTipRect.color = backgroundColorOnFocus;
                    toolTipRect.border.color = borderColorOnFocus;
                }
                if(!focus) {
                    toolTipRect.color = backgroundColor;
                    toolTipRect.border.color = borderColor;
                    if(!toolTipHitArea.containsMouse) {
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
