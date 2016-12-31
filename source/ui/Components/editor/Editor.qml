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
import "../Common/TextUtilities.js" as TextUtilities
import ClipboardAdapter 1.0

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

                    // Add capabilities for inline macros.
                    inlineMacros = inlineMacrosComponent.createObject(textArea);
                }
                visible: true
                onCursorRectangleChanged: {
                    cursorScroll(cursorRectangle);
                    var newCursorLine = (cursorRectangle.y/cursorRectangle.height);
                    newCursorLine = textArea.convertRawLineNumberToDisplayLineNumber(newCursorLine);
                    if (cursorLine !== newCursorLine) {
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
                    // Prevent moving cursor into blank line of a macro expansion. For more information, refer to ``cursorPositionChanged``.
                    else if (event.key === Qt.Key_Left || event.key === Qt.Key_Up || event.key === Qt.Key_Right || event.key === Qt.Key_Down) {
                        textArea.setTriggeringKeyEvent(event.key);
                        event.accepted = false;
                    } else if (event.key === Qt.Key_Backspace) {
                        // Prevents deleting a blank line as it would mess up the proper deletion of any
                        // macro expansion.
                        if (inlineMacros !== undefined && inlineMacros.isPositionInsideMacroBlankLine(textArea.text, textArea.cursorPosition-1)) {
                            event.accepted = true;
                        } else {
                            event.accepted = false;
                        }
                    } else {
                        textArea.setTriggeringKeyEvent(undefined);
                        event.accepted = false;
                    }
                }


                onTextChanged: {
                    //(re)start the parse timer, if an edit is made
                    if (inlineMacros !== undefined && inlineMacros.shouldUpdateText) { // Prevent restart if change was made for macro expansion.
                        editor.setTextChanged(true);
                        parseTimer.restart();
                    }

                    // Update macros.
                    if (inlineMacros !== undefined) {
                        inlineMacros.macroUpdatesOnTextChanged();
                    }
                }

                //Connection to react to editor signals
                Connections {
                    target: editor
                    onExecutionLineChanged: {
                        textArea.line = textArea.convertDisplayLineNumberToRawLineNumber(line);
                    }
                    onSetText: {
                        /* some text modifications methods of TextEdit are not available in qt 5.6,
                        *  so the text property has to be set directly.
                        */
                        textArea.text = text;
                    }
                    onForceCursorUpdate: {
                        editor.cursorLineChanged(textArea.cursorLine);
                    }
                }

                //timer for parsing
                Timer {
                    id: parseTimer
                    interval: 1000
                    repeat: false
                    onTriggered: {
                        // don't parse while executing to avoid parsing multiple
                        // times on stopping (onStopped triggers parse)
                        if(!tabView.getCurrentProjectItem().isRunning) {
                            editor.parse();
                        }
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


                // Emitted when the structure of line numbers changes (e.g. when inline macro is expanded or collapsed).
                signal lineNumberStructureChanged()

                Component {
                    id: inlineMacrosComponent
                    InlineMacros {
                    }
                }
                property var inlineMacros: undefined

                function convertRawLineNumberToDisplayLineNumber(rawLineIndex) {
                    if (inlineMacros === undefined) {
                        return rawLineIndex+1;
                    } else {
                        return inlineMacros.convertRawLineNumberToDisplayLineNumber(textArea.text, rawLineIndex);
                    }
                }

                function convertDisplayLineNumberToRawLineNumber(displayLine) {
                    if (inlineMacros === undefined) {
                        return displayLine;
                    } else {
                        return inlineMacros.convertDisplayLineNumberToRawLineNumber(displayLine);
                    }
                }

                function setTriggeringKeyEvent(keyEvent) {
                    if (inlineMacros !== undefined) {
                        inlineMacros.triggeringKeyEvent = keyEvent;
                    }
                }
            }


            // Sidebar

            Rectangle {
                id: sidebar
                focus: false
                x: container.contentX/scale.zoom
                y: 0
                height: Math.max(container.height, textArea.contentHeight)
                width: errorBar.width + lineNumbersBar.width + macroBar.width
                color: "#eeeeeb"

                property alias _macroBar: macroBar

                // Mouse area to add Breakpoints
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


                // Display errors, warnings, notes and breakpoints.
                Rectangle {
                    id: errorBar
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    width: 0.75 * textArea.cursorRectangle.height
                    color: "#00000000"

                    property var issueMarks: [{}]

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

                    // Adds a new issue of given type (error, warning, information) to the given line.
                    function addIssue(message, lineNumber, issueType) {
                        // If no issueMark exist, create a new one (issueMarks contain the actual issues
                        // for each line).
                        var newIssue;
                        if (issueMarks[lineNumber] === undefined) {
                            var issueMarkComponent = Qt.createComponent("IssueMark.qml");
                            newIssue = issueMarkComponent.createObject();
                            newIssue.y = (lineNumber-1)*textArea.cursorRectangle.height+textArea.topPadding;
                            newIssue.parent = errorBar;
                            newIssue.lineNumber = lineNumber;
                            issueMarks[lineNumber] = newIssue;
                        } else {
                            newIssue = issueMarks[lineNumber];
                        }

                        // Add a new issueItem to the issueMark.
                        newIssue.addIssueItem(message, lineNumber, issueType);

                        // Check if the issueMark's dominantIssueType should change.
                        newIssue.dominantIssueType =
                                (issueTypePriority(issueType) > issueTypePriority(newIssue.dominantIssueType))
                                ? issueType
                                : newIssue.dominantIssueType;

                        // Only errors should be expanded by default.
                        if (newIssue.dominantIssueType === "Error") {
                            newIssue.expanded = true;
                        } else {
                            newIssue.expanded = false;
                        }
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


                // Displays line numbers.
                Column {
                    id: lineNumbersBar
                    anchors.left: errorBar.right
                    y: textArea.textMargin/2
                    width: fontMetrics.averageCharacterWidth * textArea.convertRawLineNumberToDisplayLineNumber(textArea.lineCount).toString().length

                    property var currentRawLineCount: 0
                    property var _lineNumberObjects: []

                    /**
                     Updates line numbers to show given line count in a performant way.
                     \param newLineCount: New number of line numbers to display. Considered raw line numbers, i.e.
                     without factoring out blank lines for macros.
                     \param updateAll: If false, only the required amount of line numbers are created/deleted to fit the
                     newLineCount. Changes to the internal structure are ignored (e.g. when there are blank lines). If true,
                     all line numbers are updated.
                    */
                    function updateLineNumbers(newLineCount, updateAll) {
                        // If no changes since last update, don't update again.
                        if (currentRawLineCount === newLineCount && !updateAll) return;
                        // Only some lines were added or deleted; no change in structure.
                        if (updateAll === false) {
                            // Add as many lines as necessary at the bottom.
                            for (var lineIndex = currentRawLineCount; lineIndex < newLineCount; ++lineIndex) {
                                var lineNumberTextObject = lineNumberTextComponent.createObject(lineNumbersBar);
                                lineNumberTextObject.text = textArea.convertRawLineNumberToDisplayLineNumber(lineIndex);
                                _lineNumberObjects[lineIndex] = lineNumberTextObject;
                            }
                            // Remove as many lines as necessary at the bottom.
                            for (var lineIndex = currentRawLineCount; lineIndex >= newLineCount; --lineIndex) {
                                if (_lineNumberObjects[lineIndex] !== undefined && _lineNumberObjects[lineIndex] !== null) {
                                    _lineNumberObjects[lineIndex].destroy();
                                    delete _lineNumberObjects[lineIndex];
                                }
                            }
                        } else {    // Update all line numbers.
                            for (var lineIndex = 0; lineIndex < newLineCount; ++lineIndex) {
                                var lineNumber = textArea.convertRawLineNumberToDisplayLineNumber(lineIndex);
                                // Line number for this lineIndex already exist.
                                if (_lineNumberObjects[lineIndex] !== undefined && _lineNumberObjects[lineIndex] !== null) {
                                    // Line number for this lineIndex has changed, therefore delete and create new. Otherwise reuse
                                    // old object.
                                    if (_lineNumberObjects[lineIndex].text !== lineNumber.toString()) {
                                        _lineNumberObjects[lineIndex].text = lineNumber.toString();
                                    }
                                } else { // Add new line number.
                                    var lineNumberTextObject = lineNumberTextComponent.createObject(lineNumbersBar);
                                    lineNumberTextObject.text = lineNumber.toString();
                                    _lineNumberObjects[lineIndex] = lineNumberTextObject;
                                }
                                // If the line number belongs to a macro blank line, give it a lighter color. Otherwise
                                // paint it as normal (needs to be set explicitly as this line number might have been
                                // reused.
                                if (lineIndex > 0 && _lineNumberObjects[lineIndex-1].text === lineNumber.toString()) {
                                    _lineNumberObjects[lineIndex].color = "#BBBBBB";
                                } else {
                                    _lineNumberObjects[lineIndex].color = "gray";
                                }
                            }
                        }
                        // Save newLineCount as currentRawLineCount.
                        currentRawLineCount = newLineCount;
                    }

                    // Component for displaying single line number.
                    Component {
                        id: lineNumberTextComponent

                        Text {
                            color: "gray"
                            font: textArea.font
                            height: textArea.cursorRectangle.height
                        }
                    }

                    Connections {
                        target: textArea

                        // If only some lines were added or deleted (no change in structure), do light update.
                        onLineCountChanged: {
                            lineNumbersBar.updateLineNumbers(textArea.lineCount, false);
                        }

                        // If structure of line numbers changed (e.g. macro was expanded or collapsed), do
                        // full update.
                        onLineNumberStructureChanged: {
                            lineNumbersBar.updateLineNumbers(textArea.lineCount, true);
                        }

                        Component.onCompleted: {
                            lineNumbersBar.updateLineNumbers(textArea.lineCount, false);
                        }
                    }
                }



                // Displays buttons for expanding/collapsing macros.
                Rectangle {
                    id: macroBar
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: lineNumbersBar.right
                    width: 14
                    color: "#00000000"
                }


                function addBreakpoint(line) {
                    var newBreakpoint =
                            breakpointComponent.createObject(sidebar,
                                                             {"y": line*textArea.cursorRectangle.height, "line": line});
                }

                Component {
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
                            anchors.left: parent.left
                            anchors.leftMargin: height*0.15
                            anchors.verticalCenter: parent.verticalCenter
                            x: (errorBar.width - width) / 2
                            radius: width*0.5
                            color: "#0080FF"
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
            }

            // Input for zoom
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
