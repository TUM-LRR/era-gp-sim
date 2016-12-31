import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Dialogs 1.2
import "../Common"
import "../Common/TextUtilities.js" as TextUtilities
import ClipboardAdapter 1.0

Item {
    id: inlineMacros

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
            removeCurrentMacros();
            // Add the new macros.
            macros = macroList;
            addMacros();
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

    // Iterates over the current macros-array and collapses every macro.
    function collapseAllMacros() {
        for (var macroIndex = 0; macroIndex < macros.length; ++macroIndex) {
            collapseMacroSubeditor(macroIndex);
        }
        if (macros.length > 0) {
            textArea.lineNumberStructureChanged();
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
        var savedCursorPosition = textArea.cursorPosition;

        // Update line numbers if necessary (i.e. when there were macros that had to be collapsed which resulted
        // in blank lines being deleted which changed the internal line number structure).
        if (macroDisplayObjects.length > 0) {
            textArea.lineNumberStructureChanged();
        }

        macroDisplayObjects = [];

        // Restore previously saved cursor position.
        textArea.cursorPosition = savedCursorPosition;
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
            var linePosition = TextUtilities.getPositionOfOccurence(textArea.text, "\n", Number(macro["startLine"])+1);
            // Create display objects (i.e. sub-editor and triangle)
            var macroDisplayObject = {};
            // Add sub-editor to display object
            var yPos = textArea.positionToRectangle(linePosition).y + textArea.cursorRectangle.height
            var subeditor = subeditorComponent.createObject(textArea, {
                                                                "y": yPos,
                                                                "expandedHeight": textArea.cursorRectangle.height*Number(macros[macroIndex]["lineCount"]),
                                                                "text": macro["code"]});
            macroDisplayObject["subeditor"] = subeditor;
            // Add triangle-button to display object
            var triangleButton = triangleButtonComponent.createObject(sidebar._macroBar, {"y": textArea.positionToRectangle(linePosition).y-(textArea.cursorRectangle.height/5)});
            triangleButton.anchors.right = sidebar._macroBar.right;
            triangleButton.anchors.rightMargin = -1;
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
            /*// When lines where inserted or removed above any macro, its startLine has to be offset
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
            }*/
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

        // Update line numbers
        textArea.lineNumberStructureChanged();
    }

    // Expands the subeditor of the macro of given index by displaying subeditor component and inserting blank lines.
    function expandMacroSubeditor(macroIndex) {
        if (macros[macroIndex]["collapsed"] === false) { return; }

        // When the editor's text is altered, its cursor position is reset to 0. Therefore, the cursor position
        // is saved to be able to restore it later.
        var cursorPos = textArea.cursorPosition;

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

        var insertText = new Array(Number(macros[macroIndex]["lineCount"])+1).join("\n");
        // Prevent loop.
        shouldUpdateText = false;
        textArea.insert(linePosition, insertText);
        shouldUpdateText = true;

        // When inserting the blank lines somewhere ahead of the cursor, the cursor position has to be moved
        // in order to mantain its position relative to the surrounding text.
        if (cursorPos > linePosition) {
            cursorPos += (Number(macros[macroIndex]["lineCount"]) * 1);
        }

        // Move down the subeditors and triangle buttons of following macros
        for (var i = (macroIndex+1); i < macroDisplayObjects.length; i++) {
            macroDisplayObjects[i]["subeditor"].y += textArea.cursorRectangle.height * Number(macros[macroIndex]["lineCount"]);
            macroDisplayObjects[i]["triangleButton"].y += textArea.cursorRectangle.height * Number(macros[macroIndex]["lineCount"]);
        }

        // Restore cursor position.
        textArea.cursorPosition = cursorPos;
    }

    // Collapses the subeditor of the macro of given index by hiding subeditor component and removing blank lines.
    function collapseMacroSubeditor(macroIndex) {
        if (macros[macroIndex]["collapsed"] === true) { return; }

        // When the editor's text is altered, its cursor position is reset to 0. Therefore, the cursor position
        // is saved to be able to restore it later.
        var cursorPos = textArea.cursorPosition;

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
        // Prevent loop.
        shouldUpdateText = false;
        textArea.remove(linePosition, linePosition+(Number(macros[macroIndex]["lineCount"])*1));
        shouldUpdateText = true;

        // When inserting the blank lines somewhere ahead of the cursor, the cursor position has to be moved
        // in order to mantain its position relative to the surrounding text.
        if (cursorPos > linePosition) {
            cursorPos -= (Number(macros[macroIndex]["lineCount"]) * 1);
        }

        // Move down the subeditors and triangle buttons of following macros
        for (var i = (macroIndex+1); i < macroDisplayObjects.length; i++) {
            macroDisplayObjects[i]["subeditor"].y -= textArea.cursorRectangle.height * Number(macros[macroIndex]["lineCount"]);
            macroDisplayObjects[i]["triangleButton"].y -= textArea.cursorRectangle.height * Number(macros[macroIndex]["lineCount"]);
        }

        // Restore cursor position.
        textArea.cursorPosition = cursorPos;
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
        return TextUtilities.getPositionOfOccurence(textArea.text, "\n", Number(macro["startLine"])+insertedNewLines+1);
    }


    // Save cursor position to be able to restored later if needed.
    property var previousCursorPosition: 0
    // Saves the key event triggering the cursor position change. Refer to textArea's Keys.onPressed to see where this property is altered.
    property var triggeringKeyEvent: undefined
    // In order to prevent the user from moving the cursor "behind" a macro subeditor, the position is automatically altered
    // when a macro blank line is met.
    Connections {
        target: textArea

        onCursorPositionChanged: {
            // 1. Only jump when the cursor position change was triggered by an arrow key or a mouse press. For more information see Keys.onPressed.
            // 2. Only jump over blank lines if the cursor position change does not occur due to the user
            // selecting some text. Jumping would result in deselecting the text.
            // 3. Only jump if the new position would be inside a blank line soley inserted to make space for a macro expansion.
            if (triggeringKeyEvent != undefined && textArea.selectedText == "" && isPositionInsideMacroBlankLine(textArea.text, textArea.cursorPosition)) {
                var position = textArea.cursorPosition;
                // Determine whether the user moves the cursor upwards or downwards.
                var direction = (textArea.cursorPosition > previousCursorPosition) ? 1 : -1;
                // Jump over every blank line. Stop if you have reached the last line of the code.
                while (position < (textArea.text.length-1) && isPositionInsideMacroBlankLine(textArea.text, position)) {
                    position += (direction);
                }

                // Find the position of the cursor inside the just found non-blank line.
                var newPosition = position;
                // If there is no line beyond the macro expansion that is supposed to be skipped, don't move the cursor at all.
                if (newPosition >= (textArea.text.length-1)) {
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
                textArea.cursorPosition = newPosition;
            }
            previousCursorPosition = textArea.cursorPosition;
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
            if (shouldUpdateClipboardText && clipboard.text() === textArea.selectedText) {
                shouldUpdateClipboardText = false;
                // Remove any blank lines of macro expansions.
                var alteredText = clipboard.text();
                var selectionStartLine = TextUtilities.getLineNumberForPosition(alteredText, textArea.selectionStart);
                for (var macroIndex = 0; macroIndex < macros.length; ++macroIndex) {
                    var macro = macros[macroIndex];
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
}
