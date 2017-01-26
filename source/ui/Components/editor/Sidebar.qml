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


Rectangle {
  id: sidebar

  focus: false
  x: container.contentX / scale.zoom
  y: 0

  height: Math.max(container.height, textRegion.contentHeight)
  width: errorBar.width + lineNumbering.width + macroBar.width

  color: Theme.editor.sidebar.background

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
      sidebar.addBreakpoint(
        Math.floor(mouse.y / textRegion.cursorRectangle.height)
      );
    }
  }

  // Display errors, warnings, notes and breakpoints.
  Rectangle {
    id: errorBar
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    width: 0.75 * textRegion.cursorRectangle.height
    color: "#00000000"

    property var issueMarks: ({})

    Connections {
      target: editor
      onAddIssue: {
        // Note: Parser uses line numbering 1...n.
        errorBar.addIssue(message, line, color);
      }

      onDeleteErrors: errorBar.issueMarks = ({})
      Component.onCompleted: editor.init(textRegion.textDocument);
    }

    Connections {
      target: textRegion

      // A line number structure change means the structure of the visible code
      // is altered without an obligatory recompile (e.g. when macro is
      // expanded/collapsed). Therefore, the vertical position of errors has to
      // be adjusted.
      onLineNumberStructureChanged: {
        Object.keys(errorBar.issueMarks).forEach(function(key) {
          var displayLineNumber =
            textRegion.convertDisplayLineNumberToRawLineNumber(
              errorBar.issueMarks[key].lineNumber
          );
          errorBar.issueMarks[key].y = (displayLineNumber - 1)
                                     * textRegion.cursorRectangle.height
                                     + textRegion.topPadding;
        });
      }
    }

    // Adds a new issue of given type (error, warning,
    // information) to the given line.
    function addIssue(message, lineNumber, issueType) {
      // If no issueMark exist, create a new one
      // (issueMarks contain the actual issues for each line).
      var newIssue;
      if (issueMarks[lineNumber] === undefined) {
        var issueMarkComponent = Qt.createComponent("IssueMark.qml");
        newIssue = issueMarkComponent.createObject();
        newIssue.y =
          (textRegion.convertDisplayLineNumberToRawLineNumber(lineNumber) - 1)
        * textRegion.cursorRectangle.height + textRegion.topPadding;
        newIssue.parent = errorBar;
        newIssue.lineNumber = lineNumber;
        issueMarks[lineNumber] = newIssue;
      } else {
        newIssue = issueMarks[lineNumber];
      }

      // Add a new issueItem to the issueMark.
      newIssue.addIssueItem(message, lineNumber, issueType);

      // Check if the issueMark's dominantIssueType should change.
      var newPriority = issueTypePriority(newIssue.dominantIssueType);
      if (issueTypePriority(issueType) > newPriority) {
          newIssue.dominantIssueType = issueType;
      }

      // Only errors should be expanded by default.
      if (newIssue.dominantIssueType === "Error") {
        newIssue.expanded = true;
      } else {
        newIssue.expanded = false;
      }
    }

    // Assigns a priority to each issue type. Required
    // for calculating an issueMark's dominantIssueType.
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

  LineNumbering { id: lineNumbering }


  // Displays buttons for expanding/collapsing macros.
  Rectangle {
    id: macroBar
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: lineNumbering.right
    width: 0.75 * textRegion.cursorRectangle.height
    color: "#00000000"
  }

  property var breakpoints: ({})

  function addBreakpoint(lineIndex) {
    var displayLine =
      textRegion.convertRawLineNumberToDisplayLineNumber(lineIndex + 1);
    sidebar.breakpoints[displayLine] =
      breakpointComponent.createObject(sidebar, {"line": displayLine});
    sidebar.breakpoints[displayLine].y =
      lineIndex * textRegion.cursorRectangle.height;
  }

  BreakpointComponent { id: breakpointComponent }
}
