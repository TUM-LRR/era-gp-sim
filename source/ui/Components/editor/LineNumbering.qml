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
* along with this program. If not, see <http:// www.gnu.org/licenses/>.
*/

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Theme 1.0

Column {
  id: lineNumbering
  anchors.left: errorBar.right

  y: textArea.textMargin / 2
  width: {
    var lineNumber =
      textArea.convertRawLineNumberToDisplayLineNumber(textArea.lineCount);
    return fontMetrics.averageCharacterWidth * lineNumber.toString().length;
  }

  property var currentRawLineCount: 0
  property var _lineNumberObjects: []

  // Updates line numbers to show given line count in a performant way. \param
  // newLineCount: New number of line numbers to display. Considered raw line
  // numbers, i.e. without factoring out blank lines for macros. \param
  // updateAll: If false, only the required amount of line numbers are
  // created/deleted to fit the newLineCount. Changes to the internal structure
  // are ignored (e.g. when there are blank lines). If true, all line numbers
  // are updated.
  function updateLineNumbers(newLineCount, updateAll) {

    // If no changes since last update, don't update again.
    if (currentRawLineCount === newLineCount && !updateAll) return;

    // Only some lines were added or deleted; no change in structure.
    if (updateAll === false) {

      // Add as many lines as necessary at the bottom.
      for (var lineIndex = currentRawLineCount;
           lineIndex < newLineCount;
           ++lineIndex) {
        var lineNumberTextObject =
          lineNumberTextComponent.createObject(lineNumbering);

        lineNumberTextObject.text =
          textArea.convertRawLineNumberToDisplayLineNumber(lineIndex + 1);
        _lineNumberObjects[lineIndex] = lineNumberTextObject;
      }

      // Remove as many lines as necessary at the bottom.
      for (var lineIndex = currentRawLineCount;
           lineIndex >= newLineCount;
           --lineIndex) {
        if (_lineNumberObjects[lineIndex] !== undefined &&
            _lineNumberObjects[lineIndex] !== null) {
          _lineNumberObjects[lineIndex].destroy();
          delete _lineNumberObjects[lineIndex];
        }
      }
    } else {
      for (var lineIndex = 0; lineIndex < newLineCount; ++lineIndex) {
        var lineNumber =
          textArea.convertRawLineNumberToDisplayLineNumber(lineIndex + 1);

        // Line number for this lineIndex already exist.
        if (_lineNumberObjects[lineIndex] !== undefined &&
            _lineNumberObjects[lineIndex] !== null) {
          // Line number for this lineIndex has changed, therefore delete and
          // create new. Otherwise reuse old object.
          if (_lineNumberObjects[lineIndex].text !== lineNumber.toString()) {
            _lineNumberObjects[lineIndex].text = lineNumber.toString();
          }
        } else {
          var lineNumberTextObject =
            lineNumberTextComponent.createObject(lineNumbering);
          lineNumberTextObject.text = lineNumber.toString();
          _lineNumberObjects[lineIndex] = lineNumberTextObject;
        }
        // If the line number belongs to a macro blank line, give it a lighter
        // color. Otherwise paint it as normal (needs to be set explicitly as
        // this line number might have been reused.
        if (lineIndex > 0 &&
            _lineNumberObjects[lineIndex - 1].text === lineNumber.toString()) {
          _lineNumberObjects[lineIndex].color =
            Qt.lighter(Theme.editor.sidebar.lineNumber.color, 1.25);
        } else {
          _lineNumberObjects[lineIndex].color =
            Theme.editor.sidebar.lineNumber.color;
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
      color: Theme.editor.sidebar.lineNumber.color
      font: textArea.font
      height: textArea.cursorRectangle.height
    }
  }

  Connections {
    target: textArea

    // If only some lines were added or deleted (no
    // change in structure), do light update.
    onLineCountChanged: {
      lineNumbering.updateLineNumbers(textArea.lineCount, false);
    }

    // If structure of line numbers changed (e.g.
    // macro was expanded or collapsed), do full update.
    onLineNumberStructureChanged: {
      lineNumbering.updateLineNumbers(textArea.lineCount, true);
    }

    Component.onCompleted: {
      lineNumbering.updateLineNumbers(textArea.lineCount, false);
    }
  }
}
