import QtQuick 2.6

// The size of this item is the size of the tooltip trigger area
Item {
    id: toolTipItem

    // Text properties
    property alias text: _toolTipText.text
    property alias textColor: _toolTipText.color
    property alias font: _toolTipText.font
    property alias fontPointSize: _toolTipText.font.pointSize
    property alias fontPixelSize: _toolTipText.font.pixelSize
    property alias textFormat: _toolTipText.textFormat
    property alias horizontalTextAlign: _toolTipText.horizontalAlignment
    property alias wrapMode: _toolTipText.wrapMode
    property var leftPadding: _toolTipText.font.pixelSize/4
    property var rightPadding: _toolTipText.font.pixelSize/4
    property var topPadding: 0
    property var bottomPadding: 0

    // Background properties
    property color backgroundColor: Qt.rgba(239.0/255.0, 239.0/255.0, 239.0/255.0, 1.0);
    property color backgroundColorOnFocus: backgroundColor
    property color borderColor: "#666666"
    property color borderColorOnFocus: "#444444"
    property color backgroundColorOnHovered: backgroundColor
    property color borderColorOnHovered: borderColor
    property real cornerRadius: 0

    // Positioning properties
    property var relativeX: undefined           // Position of the tool tip relative to its hit area. Defaults to a position relative to mouse position.
    property var relativeY: toolTipItem.height  // Position of the tool tip relative to its hit area. Defaults to a position just below the hit area.
    property var explicitHeight: undefined
    property var explicitWidth: undefined

    // Delay for triggering the animations
    property real showDelay: 200
    property real hideDelay: 500

    // Duration of the animations
    property real showDuration: 250
    property real hideDuration: 200

    // Expose components for further customization
    property alias toolTipHitArea: _toolTipHitArea
    property alias toolTipRect: _toolTipRect
    property alias toolTipText: _toolTipText
    property alias toolTipArea: _toolTipArea

    property bool moveTooltipWithMouse: false;

    // Functions
    function showToolTip() { _toolTipHitArea.triggerShow(); }
    property var toolTipClickHandler: function (){_toolTipArea.focus = true;}   // Default implementation activates toolTipArea focus, making the tool tip stay visible.

    // MouseArea for showing the tooltip, includes the area of the parent object/specified height and length of the tooltip trigger and the tooltip itself
    MouseArea{
        id: _toolTipHitArea
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
            triggerShow();
        }

        function triggerShow() {
            if(!_toolTipRect.visible) {
                _toolTipRect.show();
                if (relativeX == undefined) {
                    _toolTipRect.x = mouseX + 10;
                } else {
                    _toolTipRect.x = relativeX;
                }
            }
            else if(_toolTipRect.visible && (hideAnimation.running || hideWithoutPause.running)){
                _toolTipRect.show();
            }
        }

        onExited: {
            if(!_toolTipArea.focus){
                _toolTipRect.hide();
            }
        }

        onPositionChanged: {
            if(moveTooltipWithMouse){
                _toolTipRect.x = mouseX + 10
                _toolTipRect.y = mouseY - 10
            }
            mouse.accepted = false;
        }

        // Rectangle around the tooltip text
        Rectangle{
            id: _toolTipRect

            // The implicit dimensions of the tool tip's background are determined by the text's
            // content dimensions and the custom padding.
            property real _implicitWidth: _toolTipText.contentWidth + leftPadding + rightPadding
            property real _implicitHeight: _toolTipText.contentHeight + topPadding + bottomPadding

            y: relativeY
            // If an explicit width/ height is specified use it, otherwise use implicitly calculated
            // dimensions.
            width: (explicitWidth == undefined) ? _implicitWidth : explicitWidth
            height: (explicitHeight == undefined) ? _implicitHeight : explicitHeight

            color: backgroundColor
            border.color: borderColor
            visible: false
            opacity: 0.0
            radius: cornerRadius

            // Text of the tooltip
            TextEdit {
                id: _toolTipText
                width: (explicitWidth == undefined) ? contentWidth : explicitWidth
                height: (explicitHeight == undefined) ? contentHeight : explicitHeight
                anchors.centerIn: parent
                clip: true
                wrapMode: TextEdit.NoWrap
                textFormat: Text.StyledText
            }

            // Showing the tooltip
            function show() {
                // Makes sure the property which is about to be animated is set to
                // a proper initial value, if it's not currently in the process of
                // showing an animation.
                if(!hideAnimation.running && !hideWithoutPause.running) {
                    _toolTipRect.opacity = 0.0;
                }

                // If the tooltip is in the process of hiding and is hovered again, it's
                // supposed to show instantly.
                if(hideAnimation.running || hideWithoutPause.running) {
                    hideAnimation.stop();
                    hideWithoutPause.stop();
                    _toolTipRect.visible = true;
                    showWithoutPause.start();
                }
                // Otherwise, animate.
                else{
                    _toolTipRect.visible = true;
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
                        target: _toolTipRect
                        property: "opacity"
                        to: 1.0
                        duration: showDuration
                    }
                }
            }

            ParallelAnimation {
                id: showWithoutPause
                NumberAnimation {
                    target: _toolTipRect
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
                        target: _toolTipRect
                        property: "opacity"
                        to: 0.0
                        duration: hideDuration
                    }
                }
                onStopped: {
                    _toolTipRect.visible = false;
                }
            }

            ParallelAnimation {
                id: hideWithoutPause
                NumberAnimation {
                    target: _toolTipRect
                    property: "opacity"
                    to: 0.0
                    duration: hideDuration
                }
                onStopped: {
                    _toolTipRect.visible = false;
                }
            }
        }


        // MouseArea the same size as the tooltip itself, for focusing on the tooltip.
        MouseArea {
            id: _toolTipArea
            x: _toolTipRect.x
            y: _toolTipRect.y
            width: _toolTipRect.width
            height: _toolTipRect.height
            visible: _toolTipRect.visible
            hoverEnabled: true
            onFocusChanged: {
                if(focus) {
                    _toolTipRect.color = backgroundColorOnFocus;
                    _toolTipRect.border.color = borderColorOnFocus;
                }
                if(!focus) {
                    _toolTipRect.color = backgroundColor;
                    _toolTipRect.border.color = borderColor;
                    if(!_toolTipHitArea.containsMouse) {
                        _toolTipRect.hide();
                    }
                }
            }
            onClicked: {
                toolTipClickHandler();
            }
            onHoveredChanged: {
                _toolTipRect.color = (containsMouse) ? backgroundColorOnHovered : backgroundColor
                _toolTipRect.border.color = (containsMouse) ? borderColorOnHovered : borderColor
            }

        }
    }
}
