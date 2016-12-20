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
import QtQuick.Dialogs 1.2
import "../Common"

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

        contentWidth: (textArea.contentWidth + sidebar.width)*scale.zoom;
        contentHeight: textArea.contentHeight*scale.zoom;

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
                property int line: 1
                property var cursorLine: 1

                width: (textArea.unscaledWidth)*scale.zoom;
                height: (textArea.unscaledHeight)*scale.zoom;

                x: sidebar.width
                selectByMouse: true
                smooth: true
                focus: true
                //tabChangesFocus: false //not working, qtbug, workaround is below
                textFormat: TextEdit.PlainText
                wrapMode: TextEdit.NoWrap
                Component.onCompleted: {
                    cursorScroll(textArea.cursorRectangle);
                }
                visible: true
                onCursorRectangleChanged: {
                    cursorScroll(cursorRectangle);
                    var newCursorLine = 1 + (cursorRectangle.y/cursorRectangle.height);
                    if(cursorLine != newCursorLine) {
                        cursorLine = newCursorLine;
                        editor.cursorLineChanged(newCursorLine);
                    }
                }

                //workaround to get tab working correctly
                Keys.onPressed: {
                    if(event.key === Qt.Key_Tab) {
                        textArea.insert(cursorPosition, "\t");
                        event.accepted = true;
                    }
                }

                // Tool tips: The desired behavior is that when the cursor is inside a keyword, the
                // keyword is underlined. Moving the mouse over this keyword makes a small help tool tip
                // appear to its right. Clicking the help tool tip opens up the help text overlay.
                // The tool tip is only created when the cursor is placed inside a keyword, saving us
                // from placing a tool tip over each keyword in advance.

                // MouseArea detecting presses inside the editor to place help tool tips. Similar behavior
                // could be achieved by using the ``cursorPositionChanged``-signal handler, however this
                // would not enable the user to close the help overlay by clicking anywhere in the editor
                // when the cursor does not thereby change its position.
                MouseArea {
                    id: toolTipTriggerArea
                    anchors.fill: parent
                    // Pass through un-handled mouse events.
                    onClicked: mouse.accepted = false;
                    onReleased: mouse.accepted = false;
                    onDoubleClicked: mouse.accepted = false;
                    onPressAndHold: mouse.accepted = false;
                    onEntered: mouse.accepted = false;

                    onPressed: {
                        parent.updateCurrentToolTip(parent.positionAt(mouseX, mouseY));
                        mouse.accepted = false;
                    }
                }


                // Currently visible tool tip. Undefined when no tool tip is visible.
                property QtObject _toolTip;
                // Checks whether a tool tip is necessary (i.e. available) for the keyword at the
                // given position and possibly creates one.
                function updateCurrentToolTip(position) {
                    // Delete old tool tip
                    _destroyCurrentToolTip();

                    // TODO: Fetch actual help-dictionary
                    var toolTipDictionary = {"mov": "mov reg1, reg2<br/>Copies the value of reg1 to reg2.",
                        "eax": "32-bit general-purpose register."};
                    // Extract the current line
                    var nextNewLine = text.substring(position).search("\n");
                    nextNewLine = (nextNewLine < 0) ? text.length : nextNewLine+position;
                    var previousNewLine = text.substring(0, position).lastIndexOf("\n");
                    previousNewLine = (previousNewLine < 0) ? 0 : previousNewLine+1;
                    var currentLine = text.substring(previousNewLine, nextNewLine);
                    // Check if any of the available keywords applies.
                    for (var searchString in toolTipDictionary) {
                        var currentPosition;            // Position of the last occurence of the current searchString.
                        var currentOffset = 0;          // In order to not always find the first occurence, the text that is being search
                        // is truncated at the beginning. The resulting offset is saved for later calculations.
                        var currentText = currentLine;  // The possibly truncated text that is being searched.
                        // Search the current line. Is truncated if multiple occurences are found.
                        while ((currentPosition = currentText.search(searchString)) != -1) {
                            // Start position of the keyword relative to the entire editor text.
                            var start = currentPosition+previousNewLine + currentOffset;
                            // End position of the keyword relative to the entire editor text.
                            var end = currentPosition+previousNewLine+currentOffset+searchString.length;
                            // Check if the cursor position lies within the keyword.
                            if (position >= start && position <= end) {
                                // Calculate the rect surrounding the keyword.
                                var startRect = positionToRectangle(start);
                                var endRect = positionToRectangle(end);
                                // Create help tool tip component.
                                var component = Qt.createComponent("HelpToolTip.qml");
                                _toolTip = component.createObject(textArea, {"helpText": toolTipDictionary[searchString],
                                                                      "x": startRect.x,
                                                                      "y": startRect.y,
                                                                      "width": (endRect.x - startRect.x),
                                                                      "height": startRect.height,
                                                                      "relativeX": (endRect.x-startRect.x+1),
                                                                      "explicitWidth": startRect.height,
                                                                      "explicitHeight": startRect.height});
                                if (_toolTip === null) {
                                    // Error Handling
                                    console.log("Error creating object");
                                }
                            }
                            // Truncate the text that is being searched for next search of the same searchString.
                            currentOffset += (currentPosition + searchString.length);
                            currentText = currentText.substring(currentPosition + searchString.length);
                        }
                    }
                }

                function _destroyCurrentToolTip() {
                    if (_toolTip != undefined) {
                        _toolTip.destroy();
                        _toolTip = undefined;
                    }
                }

                //(re)start the parse timer, if an edit is made
                onTextChanged: {
                  editor.setTextChanged(true);
                  parseTimer.restart();
                    // Remove the tooltip when the editor text is altered to prevent the situation where a help
                    // tooltip is stil visible even though the corresponding keyword has already been altered.
                    _destroyCurrentToolTip();
                }

                //Connection to react to editor signals
                Connections {
                  target: editor
                  onExecutionLineChanged: {
                    textArea.line = line;
                  }
                  onSetText: {
                    textArea.clear();
                    textArea.insert(0, text);
                  }
                }

                //timer for parsing
                Timer {
                  id: parseTimer
                  interval: 1000
                  repeat: false
                  onTriggered: {
                    editor.parse();
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

                // execution line highlighting
                Rectangle{
                    color: Qt.rgba(0.2, 0.8, 0.4, 0.2)
                    y: textArea.cursorRectangle.height * (textArea.line - 1);
                    height: textArea.cursorRectangle.height;
                    width: Math.max(scrollView.width, textArea.contentWidth)
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

                onContentSizeChanged: textArea.cursorScroll(textArea.cursorRectangle);
                Connections {
                    target: scrollView.viewport
                    onWidthChanged: textArea.cursorScroll(textArea.cursorRectangle);
                    onHeightChanged: textArea.cursorScroll(textArea.cursorRectangle);
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
                            height: textArea.cursorRectangle.height
                        }
                    }
                }

                function addBreakpoint(line) {
                  var newBreakpoint =
                  breakpointComponent.createObject(sidebar,
                    {"y": line*textArea.cursorRectangle.height, "line": line});
                }

                //mouse area to add Breakpoints
                MouseArea {
                  id: breakpointTrigger
                  width: parent.width
                  height: parent.height
                  x: 0
                  y: 0
                  z: 1
                  propagateComposedEvents: false
                  preventStealing: true
                  hoverEnabled: true
                  onClicked: {
                      sidebar.addBreakpoint(Math.floor(mouse.y/textArea.cursorRectangle.height));
                  }
                }

                Component{
                  id: breakpointComponent
                  Item {
                    z: breakpointTrigger.z + 1
                    id: breakpointItem
                    property int line;
                    property alias color: breakpointIcon.color
                    width: breakpointTrigger.width
                    height: textArea.cursorRectangle.height;
                    Component.onCompleted: {
                      editor.setBreakpoint(line + 1);
                    }

                    Rectangle {
                        id: breakpointIcon
                        height: Math.min(parent.height, errorBar.width) * 0.8
                        width: height
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: height*0.15
                        radius: width*0.5
                        color: "red"
                    }

                    MouseArea {
                      anchors.fill: parent
                      width: 100
                      height: textArea.cursorRectangle.height
                      propagateComposedEvents: false
                      preventStealing: true

                      onClicked: {
                        editor.deleteBreakpoint(line + 1);
                        breakpointItem.destroy();
                      }
                    }
                  }
                }

                //errors and warnings
                Rectangle {
                    id: errorBar
                    x: 0
                    y: textArea.textMargin/2
                    width: textArea.cursorRectangle.height*0.8

                    Connections {
                        target: editor
                        onAddError: {
                            errorBar.addError(message, line, color);
                        }

                        Component.onCompleted: {
                            editor.init(textArea.textDocument);
                        }
                    }

                    Component {
                        id: errorComponent
                        Item {
                            id: errorItem
                            property color color : "red";
                            property alias errorMessage: toolTip.text;
                            //TODO use a symbol instead of a red rectangle
                            Rectangle {
                                width: errorBar.width
                                height: fontMetrics.height
                                color: parent.color
                            }

                            //highlight the line
                            Rectangle {
                                id: lineHighlight
                                height: textArea.cursorRectangle.height
                                width: scrollView.width
                                //color: "#33ff0019"
                                color: Qt.rgba(parent.color.r, parent.color.g, parent.color.b, 0.3)
                                border.color: Qt.tint(parent.color, "#33ff3300")
                            }

                            //tooltip
                            ToolTip {
                                id: toolTip
                                width: lineHighlight.width
                                height: lineHighlight.height
                                fontPixelSize: textArea.font.pixelSize
                            }

                            Connections {
                                target: editor
                                onDeleteErrors: {
                                    errorItem.destroy();
                                }
                            }
                        }
                    }

                    function addError(message, lineNumber, errorColor) {
                        var newError = errorComponent.createObject();
                        newError.y = (lineNumber-1)*fontMetrics.height;
                        newError.parent = errorBar;
                        newError.color = errorColor;
                        newError.errorMessage = message;
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
                        textArea.cursorScroll(textArea.cursorRectangle);
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
