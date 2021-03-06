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
  id: executedLineHighlighting

  color: Theme.editor.executedLineHighlighting.background

  border.width: Theme.editor.executedLineHighlighting.border.width
  border.color: Theme.editor.executedLineHighlighting.border.color

  height: textRegion.cursorRectangle.height;
  width: Math.max(scrollView.width, textRegion.contentWidth)
  y: {
    return textRegion.cursorRectangle.height *
            (executedLineHighlighting.lineNumber - 1);
  }

  // Current raw line number to display the execution line highlight at.
  property var lineNumber

  Connections {
    target: textRegion
    onCurrentlyExecutedLineChanged: {
      executedLineHighlighting.lineNumber =
        textRegion.convertDisplayLineNumberToRawLineNumber(
          textRegion.currentlyExecutedLine
      );
    }
  }
}
