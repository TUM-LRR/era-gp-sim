/*
* C++ Assembler Interpreter
* Copyright (C) 2016 Chair of Computer Architecture
* at Technical University of Munich
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.6
import QtQuick.Controls 1.5

//decorates a Flickable with Scrollbars
ScrollView {
    id: scrollView
    verticalScrollBarPolicy: Qt.ScrollBarAsNeeded
    horizontalScrollBarPolicy: Qt.ScrollBarAsNeeded

    //background
    Rectangle {
        anchors.fill: parent
        color: "white"
    }

    //the Flickable of ScrollView
    Flickable {
        id: container
        focus: false
        anchors.fill: parent

        //wrapper item, Flickable can only have one child
        Item {
            id: item
            anchors.fill: parent

            //scaling transofmation for zooming
            transform: Scale {
                id: scale
                property real zoom: 1
                origin.x: 0;
                origin.y: 0;
                xScale: zoom;
                yScale: zoom;
                }

            //text field component
            TextEdit {
                id: textArea
                textMargin: 2
                property real unscaledWidth: Math.max(scrollView.viewport.width - sidebar.width, contentWidth)
                property real unscaledHeight: Math.max(scrollView.viewport.height, contentHeight)

                x: sidebar.width
                selectByMouse: true
                smooth: true
                focus: true
                //tabChangesFocus: false //not working, qtbug, workaround is below
                textFormat: TextEdit.PlainText
                wrapMode: TextEdit.NoWrap
                Component.onCompleted: {
                    updateSize();
                    editor.init(textDocument);
                }
                visible: true
                onCursorRectangleChanged: cursorScroll(cursorRectangle)

                //workaround to get tab working correctly
                Keys.onPressed: {
                    if(event.key === Qt.Key_Tab) {
                        textArea.insert(cursorPosition, "\t");
                        event.accepted = true;
                    }
                }

                //cursor line highlighting
                Rectangle{
                    color: Qt.rgba(0.9, 0.9, 0.9, 0.2)
                    y: textArea.cursorRectangle.y + textArea.topPadding
                    height: textArea.cursorRectangle.height
                    width: Math.max(scrollView.width, textArea.contentWidth)
                    visible: textArea.activeFocus
                    border.width: 1
                    border.color: Qt.rgba(0.7, 0.7, 0.7, 0.2)
                }

                //scroll with the cursor
                function cursorScroll(cursor) {
                    if(cursor.y < scrollView.flickableItem.contentY/scale.zoom) {
                        //up
                        scrollView.flickableItem.contentY = (cursor.y - textMargin)*scale.zoom;
                    }
                    if(cursor.y + cursor.height >= scrollView.flickableItem.contentY/scale.zoom + scrollView.viewport.height/scale.zoom - textMargin) {
                        //down
                        scrollView.flickableItem.contentY = (cursor.y + cursor.height - scrollView.viewport.height/scale.zoom + textMargin)*scale.zoom;
                    }

                    if(cursor.x < scrollView.flickableItem.contentX/scale.zoom) {
                        //left
                        scrollView.flickableItem.contentX = (cursor.x- textMargin)*scale.zoom;
                    }
                    if(cursor.x + textArea.x >= (scrollView.flickableItem.contentX/scale.zoom + scrollView.viewport.width/scale.zoom - textMargin)) {
                        //right
                        scrollView.flickableItem.contentX = (cursor.x - scrollView.viewport.width/scale.zoom + textArea.x + textMargin)*scale.zoom;
                    }
                }

                //updates the size of the TextEdit and the Flickable contentSize, has to be called when the viewport,
                //the zoom or the contentSize changes
                function updateSize() {
                    textArea.width = (textArea.unscaledWidth - sidebar.width)*scale.zoom;
                    textArea.height = (textArea.unscaledHeight)*scale.zoom;
                    container.contentWidth = (textArea.contentWidth + sidebar.width)*scale.zoom;
                    container.contentHeight = textArea.contentHeight*scale.zoom;
                    textArea.cursorScroll(textArea.cursorRectangle);
                }

                onContentSizeChanged: updateSize();
                Connections {
                    target: scrollView.viewport
                    onWidthChanged: textArea.updateSize();
                    onHeightChanged: textArea.updateSize();
                }

                Connections {
                    target: editor
                    onAddError: {
                        errorBar.addError(line, color);
                    }
                }

                //information about the font
                FontMetrics {
                    id: fontMetrics
                    font: textArea.font
                }
            }

            //area left of the TextEdit, contains lineNumbers, errorMessages, Breakpoints,...
            Rectangle {
                id: sidebar
                focus: false
                x: container.contentX/scale.zoom
                y: 0
                height: Math.max(container.height, textArea.contentHeight)
                color: "#eeeeeb"
                width: {
                    errorBar.width + fontMetrics.averageCharacterWidth + (fontMetrics.averageCharacterWidth * textArea.lineCount.toString().length);
                }

                //linenumbers
                Column {
                    id: linenumbers
                    x: 1 + errorBar.width
                    y: textArea.textMargin/2

                    Repeater {
                        model: textArea.lineCount
                        delegate: Text {
                            color: "gray"
                            font: textArea.font
                            text: index + 1
                            height: fontMetrics.height
                        }
                    }
                }

                //errors and warnings
                Rectangle {
                    id: errorBar
                    x: 0
                    y: textArea.textMargin/2
                    width: 5

                    Component {
                        id: errorComponent
                        Item {
                            id: errorItem
                            property color color : "red";
                            //TODO use a symbol instead of a red rectangle
                            Rectangle {
                                width: errorBar.width
                                height: fontMetrics.height
                                color: parent.color
                            }

                            //highlight the line
                            Rectangle {
                                height: textArea.cursorRectangle.height
                                width: scrollView.width
                                //color: "#33ff0019"
                                color: Qt.rgba(parent.color.r, parent.color.g, parent.color.b, 0.3)
                                border.color: Qt.tint(parent.color, "#33ff3300")
                            }

                            Connections {
                                target: editor
                                onDeleteErrors: {
                                    errorItem.destroy();
                                }
                            }
                        }
                    }

                    function addError(lineNumber, errorColor) {
                        var newError = errorComponent.createObject();
                        newError.y = (lineNumber-1)*fontMetrics.height;
                        newError.parent = errorBar;
                        newError.color = errorColor;
                    }
                }
            }

            //input for zoom
            MouseArea {
                id: mouseInput
                width: textArea.width
                height: textArea.height
                propagateComposedEvents: true
                scrollGestureEnabled: true
                acceptedButtons: Qt.MidButton
                onWheel: {
                    if(wheel.modifiers == Qt.ControlModifier) {
                        if(wheel.angleDelta.y > 0){
                            scale.zoom += 0.1;
                        }
                        else if(wheel.angleDelta.y < 0){
                            scale.zoom -= 0.1;
                            if(scale.zoom < 1) {
                                scale.zoom = 1.0;
                            }
                        }
                        textArea.updateSize();
                        wheel.accepted = true;
                    }
                    else {
                        wheel.accepted = false;
                    }
                }

                onClicked: wheel.accepted = false;
                onPressed: wheel.accepted = false;
                onReleased: wheel.accepted = false;
                onDoubleClicked: wheel.accepted = false;
                onPositionChanged: wheel.accepted = false;
                onPressAndHold: wheel.accepted = false;
            }
        }
    }
}
