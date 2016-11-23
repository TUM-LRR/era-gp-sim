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

                x: sidebar.width
                selectByMouse: true
                smooth: true
                focus: true
                //tabChangesFocus: false //not working, qtbug, workaround is below
                textFormat: TextEdit.PlainText
                wrapMode: TextEdit.NoWrap
                Component.onCompleted: {
                    updateSize();
                    // Temporary
                    text = ".macro huhu\naddi x1, x1, 1\naddi x1, x1, 1\n.endm\n\nhuhu\n\naddi x1, x1, 1";
                }
                visible: true
                onCursorRectangleChanged: cursorScroll(cursorRectangle)

                //workaround to get tab working correctly
                Keys.onPressed: {
                    if(event.key === Qt.Key_Tab) {
                        textArea.insert(cursorPosition, "\t");
                        event.accepted = true;
                    }
                    // Prevent moving cursor into blank line of a macro expansion. For more information, refer to ``cursorPositionChanged``.
                    else if (event.key === Qt.Key_Left || event.key === Qt.Key_Up || event.key === Qt.Key_Right || event.key === Qt.Key_Down) {
                        triggeringKeyEvent = event.key
                        event.accepted = false;
                    } else {
                        triggeringKeyEvent = undefined;
                        event.accepted = false;
                    }
                }

                //Connection to react to the parse signal
                Connections {
                  target: editor
                  onParseText: {
                    editor.sendText(textArea.text);
                  }
                  onExecutionLineChanged: {
                    textArea.line = line;
                  }
                  onRuntimeError: {
                    runtimeErrorDialog.text = errorMessage;
                    runtimeErrorDialog.open();
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

                //information about the font
                FontMetrics {
                    id: fontMetrics
                    font: textArea.font
                }



                // Macros

                property var shouldUpdateText: true

                // Information about the currently visible macros.
                property var macros: [{}]

                // Array of dictionaries each containing the display object (i.e. subeditor and triangle button)
                // for one macro.
                property var macroDisplayObjects: []

                Connections {
                    target: editor
                    onUpdateMacros: {
                        console.log("onUpdateMacros");
                        // Make sure no old macros are still present.
                        textArea.removeCurrentMacros();
                        // Add the new macros.
                        textArea.macros = macroList;
                        textArea.addMacros();
                    }
                }


                // Iterates over the current macros-arrays and removes the display objects of each macro.
                function removeCurrentMacros() {
                    // Iterate over all macro display objects and destroy them.
                    for (var macroRemovalIndex = 0; macroRemovalIndex < macroDisplayObjects.length; ++macroRemovalIndex) {
                        // Collapse the macro to delete its blank lines.
                        collapseMacroSubeditor(macroRemovalIndex);
                        // Destroy the corresponding macro display objects.
                        macroDisplayObjects[macroRemovalIndex]["subeditor"].destroy();
                        macroDisplayObjects[macroRemovalIndex]["triangleButton"].destroy();
                    }

                    // When the editor's text is altered, its cursor position is reset to 0. Therefore, the cursor position
                    // is saved to be able to restore it later.
                    var savedCursorPosition = cursorPosition;

                    macroDisplayObjects = [];

                    // Restore previously saved cursor position.
                    cursorPosition = savedCursorPosition;
                }

                // Installs new macro display objects (i.e. subeditors and triangle buttons) for each of the current
                // macros.
                function addMacros() {
                    // Save the indexes of all macros that should be expanded after their creation, to expand them all together
                    // after all macros have been created (makes moving the display objects easier to implement).
                    var macrosToExpand = [];

                    // Create display objects.
                    for (var macroIndex = 0; macroIndex < macros.length; ++macroIndex) {
                        var macro = macros[macroIndex];
                        // Find the line for the macro expansion.
                        var linePosition = TextUtilities.getPositionOfOccurence(text, "\n", macro["startLine"]+1);
                        // Create display objects (i.e. sub-editor and triangle)
                        var macroDisplayObject = {};
                        // Add sub-editor to display object
                        var yPos = textArea.positionToRectangle(linePosition).y + textArea.cursorRectangle.height
                        var subeditorComponent = Qt.createComponent("MacroSubeditor.qml");
                        var subeditor = subeditorComponent.createObject(textArea, {
                                                                 "y": yPos,
                                                                 "expandedHeight": cursorRectangle.height*macros[macroIndex]["lineCount"],
                                                                 "text": macro["code"]});
                        macroDisplayObject["subeditor"] = subeditor;
                        // Add triangle-button to display object
                        var triangleButtonComponent = Qt.createComponent("MacroTriangleButton.qml");
                        var triangleButton = triangleButtonComponent.createObject(sidebar._errorBar, {"y": textArea.positionToRectangle(linePosition).y-2, "x": 0});
                        triangleButton.macroIndex = macroIndex;
                        triangleButton.onExpandedChanged = function (currentMacroIndex){toggleExpandCollapse(currentMacroIndex);};
                        macroDisplayObject["triangleButton"] = triangleButton;
                        // Add to objects
                        macroDisplayObjects.push(macroDisplayObject);

                        // If the subeditor was previously expanded, mark it to be expanded right after its creation.
                        if (macro["collapsed"] === false) {
                            macrosToExpand.push(macroIndex);
                        }
                    }

                    // Expand all macros which are supposed to be expanded right after their creation.
                    macrosToExpand.forEach(function (value) {
                        expandMacroSubeditor(Number(value));
                    });

                    sidebar.updateLineNumbers();
                }


                // Saves the line count before a new change was made. Required for offsetting macros (see below).
                property var oldLineCount: 0

                onTextChanged: {
                    // Test is text was changed by user or the program (e.g. when expanding macro).
                    if (shouldUpdateText) {
                        // When lines where inserted or removed above any macro its startLine has to be offset
                        // in order to guarantee its blank lines are removed correctly when collapsing.
                        // Calculate the number of lines inserted/deleted.
                        var lineCountDifference = textArea.lineCount - oldLineCount;
                        // Iterate over macros which have to be offset.
                        var currentLine = TextUtilities.getLineNumberForPosition(textArea.text, textArea.cursorPosition);
                        for (var macroIndex = 0; macroIndex < macros.length; ++macroIndex) {
                            if (macros[macroIndex]["startLine"] >= (currentLine-1)) {
                                macros[macroIndex]["startLine"] += lineCountDifference;
                            }
                        }
                        // Remove macros when new text was entered.
                        removeCurrentMacros();
                    }
                    oldLineCount = textArea.lineCount;
                }



                // Called when a triangle button is pressed.
                function toggleExpandCollapse(macroIndex) {
                    if (macros[macroIndex]["collapsed"] === true) {
                        expandMacroSubeditor(macroIndex);
                    } else {
                        collapseMacroSubeditor(macroIndex);
                    }
                    sidebar.updateLineNumbers();
                }

                // Expands the subeditor of the macro of given index by displaying subeditor component and inserting blank lines.
                function expandMacroSubeditor(macroIndex) {
                    if (macros[macroIndex]["collapsed"] === false) { return; }

                    // When the editor's text is altered, its cursor position is reset to 0. Therefore, the cursor position
                    // is saved to be able to restore it later.
                    var cursorPos = cursorPosition;

                    macros[macroIndex]["collapsed"] = false;

                    // Set the button to expanded.
                    macroDisplayObjects[macroIndex]["triangleButton"].setExpanded();

                    // Expand the editor.
                    macroDisplayObjects[macroIndex]["subeditor"].expand();

                    // Insert blank lines to offset the main text.
                    var macro = macros[macroIndex];

                    // Find the line for the macro expansion.
                    var linePosition = getPositionForMacroExpansion(macroIndex);

                    // Insert empty lines to make space for the macro expansion.
                    var alteredText = [text.slice(0, linePosition), new Array(macros[macroIndex]["lineCount"]+1).join("\n"), text.slice(linePosition)].join('');
                    // Prevent loop.
                    shouldUpdateText = false;
                    text = alteredText;
                    shouldUpdateText = true;

                    // When inserting the blank lines somewhere ahead of the cursor, the cursor position has to be moved
                    // in order to mantain its position relative to the surrounding text.
                    if (cursorPos > linePosition) {
                        cursorPos += (macros[macroIndex]["lineCount"] * 1);
                    }

                    // Move down the subeditors and triangle buttons of following macros
                    for (var i = (macroIndex+1); i < macroDisplayObjects.length; i++) {
                        macroDisplayObjects[i]["subeditor"].y += cursorRectangle.height * macros[macroIndex]["lineCount"];
                        macroDisplayObjects[i]["triangleButton"].y += cursorRectangle.height * macros[macroIndex]["lineCount"];
                    }

                    // TODO: Adjust the cursor corresponding to the inserted lines (otherwise it's not where it was before).
                    cursorPosition = cursorPos;
                }

                // Collapses the subeditor of the macro of given index by hiding subeditor component and removing blank lines.
                function collapseMacroSubeditor(macroIndex) {
                    if (macros[macroIndex]["collapsed"] === true) { return; }

                    // When the editor's text is altered, its cursor position is reset to 0. Therefore, the cursor position
                    // is saved to be able to restore it later.
                    var cursorPos = cursorPosition;

                    macros[macroIndex]["collapsed"] = true;

                    // Set the button to collapsed.
                    macroDisplayObjects[macroIndex]["triangleButton"].setCollapsed();

                    // Collapse the editor.
                    macroDisplayObjects[macroIndex]["subeditor"].collapse();

                    // Remove blank lines which where inserted to offset the main text.
                    var macro = macros[macroIndex];

                    // Find the line of the macro expansion.
                    var linePosition = getPositionForMacroExpansion(macroIndex);

                    // Remove the empty lines.
                    var alteredText = TextUtilities.remove(text, linePosition, linePosition + (macros[macroIndex]["lineCount"]*1));
                    // Prevent loop.
                    shouldUpdateText = false;
                    text = alteredText;
                    shouldUpdateText = true;

                    // When inserting the blank lines somewhere ahead of the cursor, the cursor position has to be moved
                    // in order to mantain its position relative to the surrounding text.
                    if (cursorPos > linePosition) {
                        cursorPos -= (macros[macroIndex]["lineCount"] * 1);
                    }

                    // Move down the subeditors and triangle buttons of following macros
                    for (var i = (macroIndex+1); i < macroDisplayObjects.length; i++) {
                        macroDisplayObjects[i]["subeditor"].y -= cursorRectangle.height * macros[macroIndex]["lineCount"];
                        macroDisplayObjects[i]["triangleButton"].y -= cursorRectangle.height * macros[macroIndex]["lineCount"];
                    }

                    // TODO: Adjust the cursor corresponding to the removed lines (otherwise it's not where it was before).
                    cursorPosition = cursorPos;
                }


                // Returns true, if the given position is inside a blank line that was soley inserted for a macro expansion.
                function isPositionInsideMacroBlankLine(text, position) {
                    var lineNumber = TextUtilities.getLineNumberForPosition(text, position);
                    var blankLineCount = 0;
                    for (var macroIndex = 0; macroIndex < macros.length; ++macroIndex) {
                        var clearedLineNumber = lineNumber-blankLineCount;
                        if (clearedLineNumber > macros[macroIndex]["startLine"]) {
                            if (macros[macroIndex]["collapsed"] === false) {
                                if (clearedLineNumber <= macros[macroIndex]["startLine"]+macros[macroIndex]["lineCount"]) {
                                    return true;
                                }
                                blankLineCount += macros[macroIndex]["lineCount"];
                            }
                        } else {
                            return false;
                        }
                    }
                    return false;
                }

                // Finds the position where blank lines for the macro expansion have to be inserted.
                function getPositionForMacroExpansion(macroIndex) {
                    var macro = macros[macroIndex];
                    // Find the correct line of the macro expansion.
                    var insertedNewLines = 0;
                    // Blank lines added for previous macro expansions have to be considered.
                    for (var i = 0; i < macroIndex; ++i) {
                        insertedNewLines += (!macros[i]["collapsed"]) ? macros[i]["lineCount"] : 0;
                    }
                    return TextUtilities.getPositionOfOccurence(text, "\n", macro["startLine"]+insertedNewLines+1);
                }


                // Save cursor position to be able to restored later if needed.
                property var previousCursorPosition: 0
                // Saves the key event triggering the cursor position change. Refer to Keys.onPressed to see where this property is altered.
                property var triggeringKeyEvent: undefined
                // In order to prevent the user from moving the cursor "behind" a macro subeditor, the position is automatically altered
                // when a macro blank line is met.
                onCursorPositionChanged: {
                    // 1. Only jump when the cursor position change was triggered by an arrow key or a mouse press. For more information see Keys.onPressed.
                    // 2. Only jump over blank lines if the cursor position change does not occur due to the user
                    // selecting some text. Jumping would result in deselecting the text.
                    // 3. Only jump if the new position would be inside a blank line soley inserted to make space for a macro expansion.
                    if (triggeringKeyEvent != undefined && selectedText == "" && isPositionInsideMacroBlankLine(textArea.text, cursorPosition)) {
                        var position = cursorPosition;
                        // Determine whether the user moves the cursor upwards or downwards.
                        var direction = (cursorPosition > previousCursorPosition) ? 1 : -1;
                        // Jump over every blank line. Stop if you have reached the last line of the code.
                        while (position < (text.length-1) && isPositionInsideMacroBlankLine(textArea.text, position)) {
                            position += (direction);
                        }

                        // Find the position of the cursor inside the just found non-blank line.
                        var newPosition = position;
                        // If there is no line beyond the macro expansion that is supposed to be skipped, don't move the cursor at all.
                        if (newPosition >= (text.length-1)) {
                            newPosition = previousCursorPosition;
                        } else if (triggeringKeyEvent == Qt.Key_Up || triggeringKeyEvent == Qt.Key_Down) {
                            // Get the position of the previous cursor position relative to its lineCount
                            // in order to be able to use the same position in the next (non-blank) line.
                            var positionRelativeToLine = TextUtilities.getPositionRelativeToLineForPosition(textArea.text, previousCursorPosition);
                            // Calculate the new cursor position.
                            newPosition = TextUtilities.getLineStartForPosition(textArea.text, position) + positionRelativeToLine;
                            if (newPosition >= TextUtilities.getLineEndForPosition(textArea.text, position)) { newPosition = TextUtilities.getLineEndForPosition(textArea.text, position); }
                        } else if (triggeringKeyEvent == Qt.Key_Left) {
                            newPosition = TextUtilities.getLineEndForPosition(textArea.text, position);
                        } else if (triggeringKeyEvent == Qt.Key_Right) {
                            newPosition = TextUtilities.getLineStartForPosition(textArea.text, position);
                        }
                        cursorPosition = newPosition;
                    }
                    previousCursorPosition = cursorPosition;
                }
            }

            // Text can be selected as always, however the blank lines inserted for macro expansions
            // should be removed when copied. This can be done through the
            // ClipboardAdapter (Clipboard can not be accessed via QML directly).
            ClipboardAdapter {
                id: clipboard
                // Prevent loop, as clipboard is edited inside onDataChanged.
                property var shouldUpdateClipboardText: true
                // Edit copied text when clipboard data changed.
                onDataChanged: {
                    // Prevent loop.
                    if (shouldUpdateClipboardText) {
                        shouldUpdateClipboardText = false;
                        // Remove any blank lines of macro expansions.
                        var alteredText = clipboard.text();
                        var selectionStartLine = TextUtilities.getLineNumberForPosition(alteredText, textArea.selectionStart);
                        for (var macroIndex = 0; macroIndex < textArea.macros.length; ++macroIndex) {
                            var macro = textArea.macros[macroIndex];
                            if (macro["collapsed"] === false) {
                                // Calculate where the blank lines start and end, and remove the text inbetween.
                                var blankStart = TextUtilities.getLineStartForLine(alteredText, (macro["startLine"]+1-selectionStartLine));
                                var blankEnd = TextUtilities.getLineStartForLine(alteredText, (macro["startLine"]+1-selectionStartLine+macro["lineCount"]));
                                alteredText = [alteredText.slice(0, blankStart), alteredText.slice(blankEnd)].join('')
                            }
                        }
                        clipboard.setText(alteredText);
                        shouldUpdateClipboardText = true;
                    }
                }
            }


            //area left of the TextEdit, contains lineNumbers, errorMessages, Breakpoints,...
            Rectangle {
                id: sidebar
                focus: false
                x: container.contentX/scale.zoom
                y: 0
                height: Math.max(container.height, textArea.contentHeight)
                width: errorBar.width + fontMetrics.averageCharacterWidth + (fontMetrics.averageCharacterWidth * textArea.lineCount.toString().length);
                color: "#eeeeeb"

                property alias _errorBar: errorBar

                function updateLineNumbers() {
                    lineNumbersBar.updateLineNumbers();
                }

                //linenumbers
                Column {
                    id: lineNumbersBar
                    anchors.left: parent.left
                    anchors.leftMargin: 3
                    y: textArea.textMargin/2

                    property var _lineNumberObjects: []
                    function updateLineNumbers() {
                        // Delete old line numbers
                        for (var i = 0; i < _lineNumberObjects.length; ++i) {
                            _lineNumberObjects[i].destroy();
                        }
                        _lineNumberObjects = [];

                        // Add new line numbers.
                        for (var line = 0; line < textArea.lineCount; ++line) {
                            var lineNumber = Qt.createQmlObject('import QtQuick 2.6; Text {color: "gray"; font: textArea.font}',
                                                               lineNumbersBar);
                            // TODO: Find out why we need that correction factor... So strange;
                            // Not required before but now even required when using Repeater...
                            lineNumber.height = 1.05*fontMetrics.height;
                            lineNumber.text = getLineNumberForLine(line+1);
                            _lineNumberObjects[line] = lineNumber;
                        }

                    }

                    // Calculate the corresponding line number for a given line.
                    function getLineNumberForLine(line) {
                        var lineNumber = line;
                        // Find every macro that is placed above the requested line and factor out its
                        // sub-lines.
                        for (var macroIndex=0; macroIndex < textArea.macros.length; ++macroIndex) {
                            var macro = textArea.macros[macroIndex];
                            if (lineNumber > (macro["startLine"]+1)) {
                                if (macro["collapsed"] == false) {
                                    // Given line is a blank line belonging to the current macro expansion. Therefore,
                                    // no line number required.
                                    if (lineNumber <= (macro["startLine"]+1+macro["lineCount"])) {
                                        return " ";
                                    }
                                    else { // The current macro expansion lies before the given line, so the blank lines of
                                           // the macro expansion have to be factored out.
                                        lineNumber -= macro["lineCount"];
                                    }
                                }
                            }
                            // All macros lying before the given line have been considered, so the current lineNumber
                            // is correct.
                            else {
                                break;
                            }
                        }
                        return ""+lineNumber;
                    }
                }

                //errors and warnings
                Rectangle {
                    id: errorBar
                    anchors.left: lineNumbersBar.right
                    anchors.leftMargin: 1
                    y: textArea.textMargin/2
                    width: 13

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
