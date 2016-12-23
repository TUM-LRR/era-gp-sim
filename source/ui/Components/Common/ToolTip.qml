import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4

// The size of this item is the size of the tooltip trigger area
Item {
    id: toolTipItem

    // Text properties
    property alias text: _toolTipText.text
    property alias font: _toolTipText.font
    property alias fontPointSize: _toolTipText.font.pointSize
    property alias fontPixelSize: _toolTipText.font.pixelSize
    property alias textFormat: _toolTipText.textFormat
    property alias horizontalTextAlign: _toolTipText.horizontalAlignment
    property alias wrapMode: _toolTipText.wrapMode
    property var leftPadding: _toolTipText.font.pixelSize/3
    property var rightPadding: _toolTipText.font.pixelSize/3
    property var topPadding: _toolTipText.font.pixelSize/3
    property var bottomPadding: _toolTipText.font.pixelSize/3

    // Background properties
    property color backgroundColor: Qt.rgba(239.0/255.0, 239.0/255.0, 239.0/255.0, 0.8)
    property color backgroundColorOnFocus: Qt.rgba(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0)
    property color borderColor: "#DDDDDD"
    property color borderColorOnFocus: "#DDDDDD"
    property color backgroundColorOnHovered: backgroundColor
    property color borderColorOnHovered: borderColor
    property real cornerRadius: 0

    // Positioning properties
    // Position of the tool tip relative to its hit area. Defaults to a position relative to mouse position.
    property var relativeX: undefined
    // Position of the tool tip relative to its hit area. Defaults to a position just below the hit area.
    property var relativeY: toolTipItem.height
    property var explicitHeight: undefined
    property var explicitWidth: undefined
    property var maxHeight: undefined
    property var maxWidth: undefined
    readonly property alias realWidth: _toolTipRect.width
    readonly property alias realHeight: _toolTipRect.height

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

    property bool moveTooltipWithMouse: false;

    // Functions
    function showToolTip() {
      _toolTipHitArea.triggerShow();
    }

    // Default implementation activates toolTipArea focus, making the tool tip stay visible.
    property var toolTipClickHandler: function (){
        _scrollView.focus = true;
    }

    // MouseArea for showing the tooltip, includes the area of the parent object/specified height and length of the tooltip trigger and the tooltip itself
    MouseArea{
        id: _toolTipHitArea
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        acceptedButtons: Qt.LeftButton

        // Pass through un-handled mouse events.
        onPressed: mouse.accepted = false;
        onReleased: mouse.accepted = false;
        onDoubleClicked: mouse.accepted = false;
        onPressAndHold: mouse.accepted = false;

        onEntered: {
            triggerShow();
        }

        onClicked: {
            toolTipClickHandler();
            mouse.accepted = false;
        }

        onHoveredChanged: {
            _toolTipRect.color = (containsMouse) ? backgroundColorOnHovered : backgroundColor
            _toolTipRect.border.color = (containsMouse) ? borderColorOnHovered : borderColor
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
            if(!_scrollView.focus){
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
        Rectangle {
            id: _toolTipRect

            function calcSize(implicitSize, explicitSize, maxSize) {
                var size = undefined;
                if (explicitSize !== undefined) {
                    size = explicitSize;
                } else {
                    size = implicitSize;
                }
                if (maxSize !== undefined) {
                    return Math.min(size, maxSize);
                } else {
                    return size;
                }
            }

            // The implicit dimensions of the tool tip's background are determined by the text's
            // content dimensions and the custom padding.
            property var _textMaxWidth: maxWidth
            property real _implicitWidth: _textMaxWidth + leftPadding + rightPadding
            property real _implicitHeight: _toolTipText.contentHeight + topPadding + bottomPadding

            y: relativeY
            // If an explicit width/ height is specified use it, otherwise use implicitly calculated
            // dimensions.
            width: calcSize(_implicitWidth, explicitWidth, maxWidth);
            height: calcSize(_implicitHeight, explicitHeight, maxHeight);

            color: backgroundColor
            border.color: borderColor
            visible: false
            opacity: 0.0
            radius: cornerRadius

            // Text of the tooltip
            ScrollView {
                id: _scrollView
                anchors.fill: parent
                horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                style: ScrollViewStyle {
                    transientScrollBars: true
                }


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

                Flickable {
                    id: _flickable
                    anchors.fill: parent
                    contentWidth: _toolTipRect.width
                    contentHeight: _toolTipText.contentHeight + topPadding + bottomPadding

                    TextEdit {
                        id: _toolTipText
                        anchors.fill: parent
                        clip: true
                        readOnly: true
                        textFormat: Text.RichText
                        wrapMode: TextEdit.NoWrap
                        leftPadding: toolTipItem.leftPadding
                        rightPadding: toolTipItem.rightPadding
                        topPadding: toolTipItem.topPadding
                        bottomPadding: toolTipItem.bottomPadding
                        onContentWidthChanged: {
                            if(contentWidth > 0 && wrapMode === TextEdit.NoWrap) {
                                var maxTextWidth = contentWidth;
                                _toolTipRect._textMaxWidth = maxTextWidth;
                                wrapMode = TextEdit.WordWrap;
                            }
                        }
                    }
                }
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
    }
}
