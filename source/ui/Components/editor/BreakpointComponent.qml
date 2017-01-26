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


Component {
  Item {
    z: breakpointTrigger.z + 1
    id: breakpointItem
    property int line;
    property alias color: breakpointIcon.color
    width: breakpointTrigger.width
    height: textArea.cursorRectangle.height;

    Component.onCompleted: editor.setBreakpoint(line)

    Rectangle {
      id: breakpointIcon
      anchors {
        left: parent.left
        leftMargin: height * 0.15
        verticalCenter: parent.verticalCenter
      }

      height: Math.min(parent.height, errorBar.width) * 0.8
      width: height

      radius: width * Theme.editor.breakpoint.radius

      x: (errorBar.width - width) / 2
      color: Theme.editor.breakpoint.color
    }

    MouseArea {
      anchors.fill: parent
      width: 100
      height: textArea.cursorRectangle.height
      propagateComposedEvents: false
      preventStealing: true

      onClicked: {
        editor.deleteBreakpoint(line);
        delete sidebar.breakpoints[line];
        breakpointItem.destroy();
      }
    }

    Connections {
      target: textArea

      // A line number structure change means the structure of the visible code
      // is altered without an obligatory recompile (e.g. when macro is
      // expanded/collapsed). Therefore, the vertical position of breakpoints
      // has to be adjusted.
      onLineNumberStructureChanged: {
        // Update y-position with any new macro expansions/collapses in mind.
        var newY = textArea.convertDisplayLineNumberToRawLineNumber(line) - 1;
        breakpointItem.y = newY * textArea.cursorRectangle.height;
      }
    }
  }
}
