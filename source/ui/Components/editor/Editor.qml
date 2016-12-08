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
                onCursorRectangleChanged: cursorScroll(cursorRectangle)

                //workaround to get tab working correctly
                Keys.onPressed: {
                    if(event.key === Qt.Key_Tab) {
                        textArea.insert(cursorPosition, "\t");
                        event.accepted = true;
                    }
                }

                //(re)start the parse timer, if an edit is made
                onTextChanged: {
                  editor.setTextChanged(true);
                  parseTimer.restart();
                }

                //Connection to react to the parse signal
                Connections {
                  target: editor
                  onExecutionLineChanged: {
                    textArea.line = line;
                  }
                  onRuntimeError: {
                    runtimeErrorDialog.text = issueMessage;
                    runtimeErrorDialog.open();
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

            //area left of the TextEdit, contains lineNumbers, issueMessages, Breakpoints,...
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

                    property var issueMarks: ({});

                    Connections {
                        target: editor
                        onAddIssue: {
                            errorBar.addIssue(message, line, color);
                        }

                        onDeleteErrors: {
                            errorBar.issueMarks = ({});
                        }

                        Component.onCompleted: {
                            editor.init(textArea.textDocument);
                        }
                    }

                    // Each line wich contains any issue (error, warning or information) is marked by
                    // an issueMark. An issueMark is the container of one or more issueItems.
                    Component {
                        id: issueMarkComponent

                        Item {
                            id: issueMark

                            height: textArea.cursorRectangle.height
                            width: scrollView.width

                            // If the issue mark is expanded, it shows the issue icon inside the errorBar as well
                            // as the issueLineHighlight along with its issueItems (containing issue messages).
                            // If collapsed, it only shows the issue icon inside the errorBar.
                            property var expanded: true
                            // An issue mark is a container for one or more issueItems which can have varying issueTypes.
                            // Therefore the container always displays the most important issueType among all its
                            // child-issueItems (Error > Warning > Information).
                            property var dominantIssueType: ""

                            // Color definitions
                            property var errorColorSolid: Qt.rgba(1.0, 80.0/255.0, 0.0, 0.4)
                            property var errorColorLight: Qt.rgba(1.0, 80.0/255.0, 0.0, 0.14)
                            property var warningColorSolid: Qt.rgba(1.0, 185.0/255.0, 10.0/255.0, 0.4)
                            property var warningColorLight: Qt.rgba(1.0, 185.0/255.0, 10.0/255.0, 0.14)
                            property var informationColorSolid: Qt.rgba(0.0, 117.0/255.0, 255.0/255.0, 0.4)
                            property var informationColorLight: Qt.rgba(0.0, 117.0/255.0, 255.0/255.0, 0.14)

                            // The issue's Icon inside the error bar.
                            Image {
                                id: issueIcon

                                width: errorBar.width-2
                                height: errorBar.width-2
                                x: 1
                                anchors.verticalCenter: issueLineHighlight.verticalCenter
                                source: {
                                    switch (dominantIssueType) {
                                    case "Error":
                                        return "Issue Icons/Error Icon.png";
                                    case "Warning":
                                        return "Issue Icons/Warning Icon.png";
                                    case "Information":
                                        return "Issue Icons/Information Icon.png";
                                    default:
                                        return "Issue Icons/Error Icon.png";
                                    }
                                }

                                // Use issueIcon as button for expanding/collapsing issueMark.
                                MouseArea {
                                    anchors.fill: parent
                                    preventStealing: true
                                    onClicked: {
                                        expanded = !expanded;
                                    }
                                }
                            }

                            // Highlights the line the issue belongs to.
                            Rectangle {
                                id: issueLineHighlight

                                color: errorColorLight
                                visible: expanded

                                anchors.fill: parent

                                property var issueItems: []

                                // An issueItem marks an issue (error, warning, information) inside the corresponding line.
                                Component {
                                    id: issueItemComponent

                                    Rectangle {
                                        id: issueMarkTextBackground

                                        // The issue message to display.
                                        property var issueMessage: "";
                                        // The type of issue (error, warning, information).
                                        property var issueType: "";
                                        // Margin between issue item icon, issue text and right edge.
                                        property var _textMargin: 4

                                        color: {
                                            switch (issueType) {
                                            case "Error":
                                                return errorColorSolid;
                                            case "Warning":
                                                return warningColorSolid;
                                            case "Information":
                                                return informationColorSolid;
                                            default:
                                                return errorColorSolid;
                                            }
                                        }

                                        width: issueText.width + 3 * _textMargin + issueItemIcon.width

                                        // Each issueItem displays an icon corresponding to its issueType.
                                        Image {
                                            id: issueItemIcon
                                            width: errorBar.width-2
                                            height: errorBar.width-2
                                            anchors.right: issueText.left
                                            anchors.rightMargin: _textMargin
                                            anchors.verticalCenter: parent.verticalCenter
                                            source: {
                                                switch (issueType) {
                                                case "Error":
                                                    return "Issue Icons/Error Icon.png";
                                                case "Warning":
                                                    return "Issue Icons/Warning Icon.png";
                                                case "Information":
                                                    return "Issue Icons/Information Icon.png";
                                                default:
                                                    return "Issue Icons/Error Icon.png";
                                                }
                                            }
                                        }

                                        // Displays the issueMessage (error message, warning message, information message).
                                        Text {
                                            id: issueText
                                            anchors.right: parent.right
                                            anchors.rightMargin: _textMargin
                                            anchors.verticalCenter: parent.verticalCenter
                                            font.pixelSize: 0.6*parent.height
                                            text: issueMessage
                                        }

                                        // Tooltip for showing issueMessage on mouse over.
                                        ToolTip {
                                            id: toolTip
                                            width: issueLineHighlight.width
                                            height: issueLineHighlight.height
                                            fontPixelSize: textArea.font.pixelSize
                                            text: issueMessage
                                        }
                                    }

                                }

                                Connections {
                                    target: editor
                                    onDeleteErrors: {
                                        issueMark.destroy();
                                    }
                                }

                                // Adds a new issue item to the current issueMark.
                                function addIssueItem(message, issueType) {
                                    var newIssueItem = issueItemComponent.createObject();
                                    newIssueItem.parent = issueLineHighlight;
                                    newIssueItem.anchors.right = issueLineHighlight.right;
                                    newIssueItem.height = issueLineHighlight.height;
                                    newIssueItem.y = issueLineHighlight.height * (issueItems.length);
                                    newIssueItem.issueMessage = message;
                                    newIssueItem.issueType = issueType;
                                    issueItems.push(newIssueItem);
                                }
                            }

                            // Adds a new issue item to the current issueMark.
                            function addIssueItem(message, issueType) {
                                issueLineHighlight.addIssueItem(message, issueType);
                            }
                        }

                    }

                    // Adds a new issue of given type (error, warning, information) to the given line.
                    function addIssue(message, lineNumber, issueType) {
                        // If no issueMark exist, create a new one (issueMarks contain the actual issues
                        // for each line).
                        var newIssue;
                        if (issueMarks[lineNumber] === undefined) {
                            newIssue = issueMarkComponent.createObject();
                            newIssue.y = (lineNumber-1)*fontMetrics.height*1.05;
                            newIssue.parent = errorBar;
                            issueMarks[lineNumber] = newIssue;
                        } else {
                            newIssue = issueMarks[lineNumber];
                        }
                        // Add a new issueItem to the issueMark.
                        newIssue.addIssueItem(message, issueType);
                        // Check if the issueMark's dominantIssueType should change.
                        newIssue.dominantIssueType =
                                (issueTypePriority(issueType) > issueTypePriority(newIssue.dominantIssueType))
                                ? issueType
                                : newIssue.dominantIssueType;
                    }

                    // Assigns a priority to each issue type. Required for calculating an issueMark's
                    // dominantIssueType.
                    function issueTypePriority(issueType) {
                        switch (issueType) {
                        case "Error":
                            return 3;
                        case "Warning":
                            return 2;
                        case "Information":
                            return 1;
                        default:
                            return 0;
                        }
                    }
                }
            }

            //Dialog to show runtime errors
            MessageDialog {
              id: runtimeErrorDialog
              title: "Runtime error"
              standardButtons: StandardButton.Ok
              onAccepted: {
                close();
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
