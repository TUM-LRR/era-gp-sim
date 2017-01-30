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

TextEdit {
  id: textRegion
  x: sidebar.width
  textMargin: 2
  selectByMouse: true
  smooth: true
  focus: true
  font.pointSize: Theme.editor.pointSize

  textFormat: TextEdit.PlainText
  wrapMode: TextEdit.NoWrap
  color: Theme.editor.color

  // Line that is going to be executed in the next step.
  property int currentlyExecutedLine: 0
  property var cursorLine: 1

  ContextMenu { }

  Component.onCompleted: {
    cursorScroll(textRegion.cursorRectangle);
    inlineMacros = inlineMacrosComponent.createObject(textRegion);
  }

  onCursorRectangleChanged: {
    cursorScroll(cursorRectangle);

    var newCursorLine = (cursorRectangle.y / cursorRectangle.height) + 1;
    newCursorLine =
      textRegion.convertRawLineNumberToDisplayLineNumber(newCursorLine);
    if (cursorLine !== newCursorLine) {
      cursorLine = newCursorLine;
      editor.cursorLineChanged(newCursorLine);
    }

    updateHelpTooltip();
  }

  Keys.onPressed: {
    // If the user presses a key which potentially changes textRegion's text,
    // collapse all macros to prevent the text change interfering with macro
    // expansions.
    if (event.key < 0x01000010 || event.key > 0x01000060) {
      if (inlineMacros !== undefined) {
        inlineMacros.collapseAllMacros();
      }
    }

    if (event.key === Qt.Key_Tab) {
      textRegion.insert(cursorPosition, "\t");
      event.accepted = true;
    } else if (event.key === Qt.Key_Left  ||
              event.key === Qt.Key_Up     ||
              event.key === Qt.Key_Right  ||
              event.key === Qt.Key_Down) {
      // Prevent moving cursor into blank line of a macro expansion. For more
      // information, refer to ``cursorPositionChanged``.
      textRegion.setTriggeringKeyEvent(event.key);
      event.accepted = false;
    } else {
      textRegion.setTriggeringKeyEvent(undefined);
      event.accepted = false;
    }
  }

  // Tooltips: A small '?' Symbol is placed under the cursor, if there is help
  // available. By hovering over it, the help text can be overlayed.
  function updateHelpTooltip() {
    var help = guiProject.getCommandHelp(cursorLine);
    if (help.length === 0) {
      _toolTip.hideIcon();
      return;
    }

    _toolTip.x = cursorRectangle.x + x;
    _toolTip.y = cursorRectangle.y + cursorRectangle.height - 1
    _toolTip.width = cursorRectangle.height;
    _toolTip.height = cursorRectangle.height;
    _toolTip.helpText = help;
    _toolTip.showIcon();
  }

  onTextChanged: {
    // (Re-)start the parse timer, if an edit is made
    if (inlineMacros !== undefined && inlineMacros.shouldUpdateText) {
      // Prevent restart if change was made for macro expansion.
      editor.setTextChanged(true);
      parseTimer.restart();
    }

    if (inlineMacros !== undefined) {
      inlineMacros.macroUpdatesOnTextChanged();
    }
  }

  // A line number structure change means the structure of the
  // visible code is altered without an obligatory recompile (e.g.
  // when macro is expanded/collapsed). Therefore, the vertical
  // position of errors has to be adjusted.
  onLineNumberStructureChanged: {
    // Update execution line to consider new macro expansion.
    executedLineHighlighting.lineNumber =
      textRegion.convertDisplayLineNumberToRawLineNumber(
        textRegion.currentlyExecutedLine
    );
  }

  Connections {
    target: editor
    onExecutionLineChanged: textRegion.currentlyExecutedLine = line

    // Some text modifications methods of TextEdit are not available in Qt
    // 5.6, so the text property has to be set directly.
    onSetText: textRegion.text = text

    onForceCursorUpdate: editor.cursorLineChanged(textRegion.cursorLine)

    // Before the editor text is used by any other module, collapse all macros
    // to prevent blank lines from appearing inside the code.
    onPrepareTextForRetrieval: textRegion.inlineMacros.collapseAllMacros();
  }

  Connections {
    target: guiProject
    onCommandListUpdated: textRegion.updateHelpTooltip();
  }

  Timer {
    id: parseTimer
    interval: 1000
    repeat: false
    onTriggered: {
      // Don't parse while executing to avoid parsing multiple
      // times on stopping (onStopped triggers parse).
      if (!tabView.currentProjectItem().running) {
        editor.parse();
      }
    }
  }

  CursorLineHighlighting { }
  ExecutedLineHighlighting { id: executedLineHighlighting }


  // Scroll with the cursor
  function cursorScroll(cursor) {
    scrollUp(cursor);
    scrollDown(cursor);
    scrollLeft(cursor);
    scrollRight(cursor);
  }

  function scrollUp(cursor) {
    if (cursor.y < (scrollView.flickableItem.contentY / scale.zoom)) {
      scrollView.flickableItem.contentY = (cursor.y - textMargin) * scale.zoom;
    }
  }

  function scrollDown(cursor) {
    var viewportHeight = scrollView.viewport.height / scale.zoom;
    var bottomBoundary = scrollView.flickableItem.contentY / scale.zoom
                       + viewportHeight - textMargin;
    if (cursor.y + cursor.height >= bottomBoundary) {
      var newY = cursor.y + cursor.height - viewportHeight + textMargin;
      scrollview.flickableItem.contentY = newY * scale.zoom;
    }
  }

  function scrollLeft(cursor) {
    if (cursor.x < scrollView.flickableItem.contentX / scale.zoom) {
      scrollView.flickableItem.contentX = (cursor.x - textMargin) * scale.zoom;
    }
  }

  function scrollRight(cursor) {
    var topBoundary = (scrollView.flickableItem.contentX / scale.zoom)
                    + (scrollView.viewport.width / scale.zoom)
                    - textMargin;
    if (cursor.x + textRegion.x >= topBoundary) {
      var newX = cursor.x
               - (scrollView.viewport.width / scale.zoom)
               + textRegion.x + textMargin;
      scrollView.flickableItem.contentX = newX * scale.zoom;
    }
  }


  onContentSizeChanged: textRegion.cursorScroll(textRegion.cursorRectangle);
  Connections {
    target: scrollView.viewport
    onWidthChanged: textRegion.cursorScroll(textRegion.cursorRectangle);
    onHeightChanged: textRegion.cursorScroll(textRegion.cursorRectangle);
  }


  // Emitted when the structure of line numbers changes
  // (e.g. when inline macro is expanded or collapsed).
  signal lineNumberStructureChanged()

  Component {
    id: inlineMacrosComponent
    InlineMacros { }
  }

  property var inlineMacros: undefined

  function convertRawLineNumberToDisplayLineNumber(rawLine) {
    if (inlineMacros === undefined) {
      return rawLine;
    } else {
      return inlineMacros.convertRawLineNumberToDisplayLineNumber(
        textRegion.text,
        rawLine
      );
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
