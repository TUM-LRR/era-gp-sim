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
                }
                visible: true
                onCursorRectangleChanged: {
                    cursorScroll(cursorRectangle);
                    var newCursorLine = (cursorRectangle.y/cursorRectangle.height);
                    newCursorLine = convertRawLineNumberToDisplayLineNumber(textArea.text, newCursorLine);
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
                        triggeringKeyEvent = event.key
                        event.accepted = false;
                    } else if (event.key === Qt.Key_Backspace) {
                        // Prevents deleting a blank line as it would mess up the proper deletion of any
                        // macro expansion.
                        if (isPositionInsideMacroBlankLine(textArea.text, textArea.cursorPosition-1)) {
                            event.accepted = true;
                        } else {
                            event.accepted = false;
                        }
                    } else {
                        triggeringKeyEvent = undefined;
                        event.accepted = false;
                    }
                }


                onTextChanged: {
                    //(re)start the parse timer, if an edit is made
                    if (shouldUpdateText) { // Prevent restart if change was made for macro expansion.
                        editor.setTextChanged(true);
                        parseTimer.restart();
                    }

                    // Update macros.
                    macroUpdatesOnTextChanged();
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
                        // Make sure no old macros are still present.
                        textArea.removeCurrentMacros();
                        // Add the new macros.
                        textArea.macros = macroList;
                        textArea.addMacros();
                    }
                }

                Component {
                    id: subeditorComponent
                    MacroSubeditor {
                    }
                }

                Component {
                    id: triangleButtonComponent
                    MacroTriangleButton {
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
                        var linePosition = TextUtilities.getPositionOfOccurence(text, "\n", Number(macro["startLine"])+1);
                        // Create display objects (i.e. sub-editor and triangle)
                        var macroDisplayObject = {};
                        // Add sub-editor to display object
                        var yPos = textArea.positionToRectangle(linePosition).y + textArea.cursorRectangle.height
                        var subeditor = subeditorComponent.createObject(textArea, {
                                                                                 "y": yPos,
                                                                                 "expandedHeight": cursorRectangle.height*Number(macros[macroIndex]["lineCount"]),
                                                                                 "text": macro["code"]});
                        macroDisplayObject["subeditor"] = subeditor;
                        // Add triangle-button to display object
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

                }


                // Saves the line count before a new change was made. Required for offsetting macros (see below).
                property var oldLineCount: 0

                function macroUpdatesOnTextChanged() {
                    // Only update when text was changed by user but not by the program (e.g. when expanding macro).
                    if (shouldUpdateText) {
                        // When lines where inserted or removed above any macro, its startLine has to be offset
                        // in order to guarantee its blank lines are removed correctly when collapsing.
                        // Calculate the number of lines inserted/deleted.
                        var lineCountDifference = textArea.lineCount - oldLineCount;
                        // Iterate over macros which have to be offset.
                        var currentLine = TextUtilities.getLineNumberForPosition(textArea.text, textArea.cursorPosition);
                        currentLine = convertRawLineNumberToDisplayLineNumber(textArea.text, currentLine);
                        for (var macroIndex = 0; macroIndex < macros.length; ++macroIndex) {
                            if (Number(macros[macroIndex]["startLine"]) >= (currentLine-1)) {
                                var newStartLine = Number(macros[macroIndex]["startLine"]) + lineCountDifference;
                                macros[macroIndex]["startLine"] = newStartLine;
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
                    var alteredText = [text.slice(0, linePosition), new Array(Number(macros[macroIndex]["lineCount"])+1).join("\n"), text.slice(linePosition)].join('');
                    // Prevent loop.
                    shouldUpdateText = false;
                    text = alteredText;
                    shouldUpdateText = true;

                    // When inserting the blank lines somewhere ahead of the cursor, the cursor position has to be moved
                    // in order to mantain its position relative to the surrounding text.
                    if (cursorPos > linePosition) {
                        cursorPos += (Number(macros[macroIndex]["lineCount"]) * 1);
                    }

                    // Move down the subeditors and triangle buttons of following macros
                    for (var i = (macroIndex+1); i < macroDisplayObjects.length; i++) {
                        macroDisplayObjects[i]["subeditor"].y += cursorRectangle.height * Number(macros[macroIndex]["lineCount"]);
                        macroDisplayObjects[i]["triangleButton"].y += cursorRectangle.height * Number(macros[macroIndex]["lineCount"]);
                    }

                    // Restore cursor position.
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
                    var alteredText = TextUtilities.remove(text, linePosition, linePosition + (Number(macros[macroIndex]["lineCount"])*1));
                    // Prevent loop.
                    shouldUpdateText = false;
                    text = alteredText;
                    shouldUpdateText = true;

                    // When inserting the blank lines somewhere ahead of the cursor, the cursor position has to be moved
                    // in order to mantain its position relative to the surrounding text.
                    if (cursorPos > linePosition) {
                        cursorPos -= (Number(macros[macroIndex]["lineCount"]) * 1);
                    }

                    // Move down the subeditors and triangle buttons of following macros
                    for (var i = (macroIndex+1); i < macroDisplayObjects.length; i++) {
                        macroDisplayObjects[i]["subeditor"].y -= cursorRectangle.height * Number(macros[macroIndex]["lineCount"]);
                        macroDisplayObjects[i]["triangleButton"].y -= cursorRectangle.height * Number(macros[macroIndex]["lineCount"]);
                    }

                    // Restore cursor position.
                    cursorPosition = cursorPos;
                }

                // Returns true, if the given position is inside a blank line that was soley inserted for a macro expansion.
                function isPositionInsideMacroBlankLine(text, position) {
                    var lineNumber = TextUtilities.getLineNumberForPosition(text, position);
                    var blankLineCount = 0;
                    // Iterates over each macro and checks its blank lines. If a blank line is found that corresponds to
                    // the given lineNumber, true is returned. If at some point the macro's line number surpasses the
                    // given lineNumber, false is returned, as it is save to assume that lineNumber does not correspond
                    // to a blank line.
                    for (var macroIndex = 0; macroIndex < macros.length; ++macroIndex) {
                        var clearedLineNumber = lineNumber-blankLineCount;
                        if (clearedLineNumber > Number(macros[macroIndex]["startLine"])) {
                            if (macros[macroIndex]["collapsed"] === false) {
                                // Given lineNumber lies within an macro expansion's blank lines.
                                if (clearedLineNumber <= Number(macros[macroIndex]["startLine"])+Number(macros[macroIndex]["lineCount"])) {
                                    return true;
                                }
                                // Save number of blank lines to filter them out when reading macro-startLine information.
                                blankLineCount += Number(macros[macroIndex]["lineCount"]);
                            }
                        } else {
                            return false;
                        }
                    }
                    // Given lineNumber lies below any macro and therefore cannot correspond to a blank line.
                    return false;
                }

                // Accepts a given line number inside the editor and factors out all blank lines inserted for macro expansions.
                function convertRawLineNumberToDisplayLineNumber(text, rawLineNumber) {
                    var lineNumber = 0;
                    for (var lineIndex = 0; lineIndex <= rawLineNumber; ++lineIndex) {
                        if (!isPositionInsideMacroBlankLine(text, TextUtilities.getLineStartForLine(text, lineIndex))) {
                            lineNumber++;
                        }
                    }
                    return lineNumber;
                }

                // Accepts a given display line number (i.e. as in sidebar) and factors in all blank lines inserted for macro expansions.
                function convertDisplayLineNumberToRawLineNumber(displayLineNumber) {
                    var rawLineNumber = displayLineNumber;
                    for (var macroIndex = 0; macroIndex < macros.length; ++macroIndex) {
                        var macro = macros[macroIndex];
                        if (Number(macro["startLine"]) < displayLineNumber && macro["collapsed"] === false) {
                            rawLineNumber += Number(macro["lineCount"]);
                        }
                    }
                    return rawLineNumber;
                }

                // Finds the position where blank lines for the macro expansion have to be inserted.
                function getPositionForMacroExpansion(macroIndex) {
                    var macro = macros[macroIndex];
                    // Find the correct line of the macro expansion.
                    var insertedNewLines = 0;
                    // Blank lines added for previous macro expansions have to be considered.
                    for (var i = 0; i < macroIndex; ++i) {
                        insertedNewLines += (!macros[i]["collapsed"]) ? Number(macros[i]["lineCount"]) : 0;
                    }
                    return TextUtilities.getPositionOfOccurence(text, "\n", Number(macro["startLine"])+insertedNewLines+1);
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
            // ClipboardAdapter (Clipboard cannot be accessed via QML directly).
            ClipboardAdapter {
                id: clipboard
                // Prevents loop, as clipboard is edited inside onDataChanged.
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
                                var blankStart = TextUtilities.getLineStartForLine(alteredText, (Number(macro["startLine"])+1-selectionStartLine));
                                var blankEnd = TextUtilities.getLineStartForLine(alteredText, (Number(macro["startLine"])+1-selectionStartLine+Number(macro["lineCount"])));
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

                    Repeater {
                        model: textArea.lineCount
                        delegate: Text {
                            color: "gray"
                            font: textArea.font
                            text: {
                                // Check if line number belongs to line which was inserted for a macro expansion.
                                if (textArea.isPositionInsideMacroBlankLine(textArea.text, TextUtilities.getLineStartForLine(textArea.text, index))) {
                                    return " ";
                                } else {    // If not, return line number with blank lines factored out.
                                    return textArea.convertRawLineNumberToDisplayLineNumber(textArea.text, index);
                                }
                            }
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
                    width: errorBar.width
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
                    anchors.left: lineNumbersBar.right
                    anchors.leftMargin: 1
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
