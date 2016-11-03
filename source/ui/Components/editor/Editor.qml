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
                    // TEMP
                    text = "mov eax, 5\nmov ebx, 2\nmacro blub\nsub ebx, 2\nmacro bluh"
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

                // Macros are updated when the text changes. However, updating the macros changes the text which would
                // result in an infinite loop. Therefore this variable prevents macro updates when the text change occurs
                // due to updating the macros.
                property var shouldUpdateMacros: true

                // Information about the currently visible macros.
                property var macros: []

                // Array of dictionaries each containing the display object (i.e. subeditor and triangle button)
                // for one macro.
                property var macroDisplayObjects: []

                // A hopefully unique keyword used to quickly identify lines which where soley inserted to make space for a
                // macro expansion (see expandMacroSubeditor:).
                property var kBlankLineKeyword: "\u0488"


                // Removes all previously installed macro display objects (i.e. subeditors and triangle buttons)
                // and installs new ones depending on the current text.
                function updateMacros() {
                    // Prevent loop (for more information refer to shouldUpdateMacros definition).
                    shouldUpdateMacros = false;

                    // Remove old macros.
                    for (var macroRemovalIndex = 0; macroRemovalIndex < macros.length; ++macroRemovalIndex) {
                        // Destroy the corresponding macro display objects.
                        macroDisplayObjects[macroRemovalIndex]["subeditor"].destroy();
                        macroDisplayObjects[macroRemovalIndex]["triangleButton"].destroy();
//                        // Remove the lines that where previously added to
//                        if (macros[macroRemovalIndex]["collapsed"] === false) {
//                            var lineRemovalPosition = getPositionOfOccurence(text, "\n", macros[macroRemovalIndex]["line"]+1);
//                            text = remove(text, lineRemovalPosition, lineRemovalPosition+macros[macroRemovalIndex]["lineCount"]);
//                        }
                    }

                    // When the editor's text is altered, its cursor position is reset to 0. Therefore, the cursor position
                    // is saved to be able to restore it later.
                    var cursorPos = cursorPosition;

                    // Remove the blank lines that where previously added (each blank line contains the kBlankLineKeyword for quick
                    // identification).
                    text = text.replace(new RegExp("\n" + kBlankLineKeyword, "g"), "");
                    macroDisplayObjects = [];

                    cursorPosition = cursorPos;

                    // TODO: Fetch actual macros from core
                    macros = [{"line": 2, "lineCount": 3, "text": "mov eax, 0\nmov ebx, 1\nadd ex, 2", "collapsed": false},
                                  {"line": 4, "lineCount": 2, "text": "add eax, 2\nsub ebx, 2", "collapsed": true}];
                    // Update to actual macro line numbers. Required as macros are currently still hardcoded (see above).
                    // Depending on how finding the macros will be implemented later, this may not be required.
                    var offset = 0;
                    var index = 0;
                    var position = -1;
                    while ((position = textArea.text.indexOf("macro", offset)) != -1) {
                        macros[index]["line"] = getLineNumberForPosition(position);
                        index++;
                        offset += (position + 5);
                    }

                    // Save the indexes of all macros that should be expanded after their creation, to expand them all together
                    // after all macros have been created (makes moving the display objects easier to implement).
                    var macrosToExpand = [];

                    // Create display objects.
                    for (var macroIndex = 0; macroIndex < macros.length; ++macroIndex) {
                        var macro = macros[macroIndex];
                        // Find the line for the macro expansion.
                        var linePosition = getPositionOfOccurence(text, "\n", macro["line"]+1);
                        // Create display objects (i.e. sub-editor and triangle)
                        var macroDisplayObject = {};
                        // Add sub-editor to display object
                        var yPos = textArea.positionToRectangle(linePosition).y + textArea.cursorRectangle.height
                        var subeditorComponent = Qt.createComponent("MacroSubeditor.qml");
                        var subeditor = subeditorComponent.createObject(textArea, {
                                                                 "y": yPos,
                                                                 "expandedHeight": cursorRectangle.height*macro["lineCount"],
                                                                 "text": macro["text"]});
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

                    shouldUpdateMacros = true;
                }

                onTextChanged: {
                    // Prevent loop.
                    if (shouldUpdateMacros) {
                        updateMacros();
                    }
                }

                // Called when a triangle button is pressed.
                function toggleExpandCollapse(macroIndex) {
                    if (macros[macroIndex]["collapsed"] == true) {
                        expandMacroSubeditor(macroIndex);
                    } else {
                        collapseMacroSubeditor(macroIndex);
                    }
                    sidebar.updateLineNumbers();
                }

                // Note: Maybe initially expanding macros can be sped up by starting with the last macro (no offset to consider).

                function expandMacroSubeditor(macroIndex) {
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
                    var alteredText = [text.slice(0, linePosition), new Array(macro["lineCount"]+1).join("\n" + kBlankLineKeyword), text.slice(linePosition)].join('');
                    // Prevent loop.
                    shouldUpdateMacros = false;
                    text = alteredText;
                    shouldUpdateMacros = true;

                    // When inserting the blank lines somewhere ahead of the cursor, the cursor position has to be moved
                    // in order to mantain its position relative to the surrounding text.
                    if (cursorPos > linePosition) {
                        cursorPos += (macro["lineCount"] * (kBlankLineKeyword.length + 1));
                    }

                    // Move down the subeditors and triangle buttons of following macros
                    for (var i = (macroIndex+1); i < macroDisplayObjects.length; i++) {
                        macroDisplayObjects[i]["subeditor"].y += cursorRectangle.height * macro["lineCount"];
                        macroDisplayObjects[i]["triangleButton"].y += cursorRectangle.height * macro["lineCount"];
                    }

                    // TODO: Adjust the cursor corresponding to the inserted lines (otherwise it's not where it was before).
                    cursorPosition = cursorPos;
                }

                function collapseMacroSubeditor(macroIndex) {
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
                    var alteredText = remove(text, linePosition, linePosition + (macro["lineCount"]*(kBlankLineKeyword.length+1)));
                    // Prevent loop.
                    shouldUpdateMacros = false;
                    text = alteredText;
                    shouldUpdateMacros = true;

                    // When inserting the blank lines somewhere ahead of the cursor, the cursor position has to be moved
                    // in order to mantain its position relative to the surrounding text.
                    if (cursorPos > linePosition) {
                        cursorPos -= (macro["lineCount"] * (kBlankLineKeyword.length + 1));
                    }

                    // Move down the subeditors and triangle buttons of following macros
                    for (var i = (macroIndex+1); i < macroDisplayObjects.length; i++) {
                        macroDisplayObjects[i]["subeditor"].y -= cursorRectangle.height * macro["lineCount"];
                        macroDisplayObjects[i]["triangleButton"].y -= cursorRectangle.height * macro["lineCount"];
                    }

                    // TODO: Adjust the cursor corresponding to the removed lines (otherwise it's not where it was before).
                    cursorPosition = cursorPos;
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
                    return getPositionOfOccurence(text, "\n", macro["line"]+insertedNewLines+1);
                }

                // Returns the number of the line the given position belongs to.
                function getLineNumberForPosition(position) {
                    return numberOfOccurences(textArea.text.slice(0, position), "\n");
                }

                // Returns the line text of the line the given position belongs to.
                function getLineForPosition(position) {
                    var lineStart = getLineStartForPosition(position);
                    var lineEnd = getLineEndForPosition(position);
                    return textArea.text.slice(lineStart, lineEnd);
                }

                // Returns the position of the start of the line relative to the entire editor text.
                function getLineStartForPosition(position) {
                    var lineStart = textArea.text.slice(0, position).lastIndexOf("\n") + 1;
                    if (lineStart == -1) { lineStart = 0; }
                    return lineStart;
                }

                // Returns the position of the end of the line (i.e. the next character would be the newline character)
                // relative to the entire editor text.
                function getLineEndForPosition(position) {
                    var lineEnd = textArea.text.slice(position).indexOf("\n") + position;
                    if (lineEnd == -1) { lineEnd = textArea.text.length; }
                    return lineEnd;
                }

                // Converts a given position relative to the entire text to a position relative to the line of the
                // given position.
                function getPositionRelativeToLineForPosition(position) {
                    var lineStart = getLineStartForPosition(position);
                    return position - lineStart;
                }

                // Returns the position of the ``n``-th occurence of the string ``searchString`` inside ``text``.
                function getPositionOfOccurence(string, searchString, n) {
                   return string.split(searchString, n).join(searchString).length;
                }

                // Returns a string with the substring beginning at a (inclusive) and ending at b (exclusive) removed from text.
                function remove(string, a, b) {
                    return string.slice(0, a) + string.slice(b, text.length);
                }

                // Returns the number of occurences of a given searchString inside a given text.
                function numberOfOccurences(string, searchString) {
                    string += "";
                    searchString += "";
                    if (searchString.length <= 0) return (string.length + 1);

                    var index = -1;
                    var offset = 0;
                    var count = 0;
                    while ((index = string.indexOf(searchString, offset)) != -1) {
                        count++;
                        offset = (index + searchString.length);
                    }
                    return count;
                }


                property var previousCursorPosition: 0
                property var triggeringKeyEvent: undefined
                onCursorPositionChanged: {
                    // 1. Only jump when the cursor position change was triggered by an arrow key or a mouse press. For more information see Keys.onPressed.
                    // 2. Only jump over blank lines if the cursor position change does not occur due to the user
                    // selecting some text. Jumping would result in deselecting the text.
                    // 3. Additional condition required to prevent loop, as cursorPosition is changed inside.
                    if (triggeringKeyEvent != undefined && selectedText == "" && getLineForPosition(cursorPosition).indexOf(kBlankLineKeyword) != -1) {
                        var position = cursorPosition;
                        // Determine whether the user moves the cursor upwards or downwards.
                        var direction = (cursorPosition > previousCursorPosition) ? 1 : -1;
                        // Jump over every blank line.
                        while (getLineForPosition(position).indexOf(kBlankLineKeyword) !== -1) {
                            position += (direction * (kBlankLineKeyword.length + 1));
                        }

                        // Find the position of the cursor inside the just found non-blank line.
                        var newPosition = position;
                        if (triggeringKeyEvent == Qt.Key_Up || triggeringKeyEvent == Qt.Key_Down) {
                            // Get the position of the previous cursor position relative to its lineCount
                            // in order to be able to use the same position in the next (non-blank) line.
                            var positionRelativeToLine = getPositionRelativeToLineForPosition(previousCursorPosition);
                            // Calculate the new cursor position.
                            newPosition = getLineStartForPosition(position) + positionRelativeToLine;
                            if (newPosition >= getLineEndForPosition(position)) { newPosition = getLineEndForPosition(position); }
                        } else if (triggeringKeyEvent == Qt.Key_Left) {
                            newPosition = getLineEndForPosition(position);
                        } else if (triggeringKeyEvent == Qt.Key_Right) {
                            newPosition = getLineStartForPosition(position);
                        }
                        cursorPosition = newPosition;
                    }
                    previousCursorPosition = cursorPosition;
                }
            }

            // Text can be selected as always, however the blank lines inserted for macro expansion and
            // the kBlankLineKeywords should be removed when copied. This can be done through the
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
                        // Remove any blank lines and the corresponding kBlankLineKeywords.
                        var alteredText = clipboard.text().replace(new RegExp("\n" + textArea.kBlankLineKeyword, "g"), "");
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
                            if (lineNumber > (macro["line"]+1)) {
                                if (macro["collapsed"] == false) {
                                    // Given line is a blank line belonging to the current macro expansion. Therefore,
                                    // no line number required.
                                    if (lineNumber <= (macro["line"]+1+macro["lineCount"])) {
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
