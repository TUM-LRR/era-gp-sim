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
import Theme 1.0
import "../Common"
import "../../Js/TextUtility.js" as TextUtility

// Notes on line numbering: The editor displays macro expansions inline which
// means that code below a macroBar expansion that was expanded (i.e. is
// visible) has to be offset in order to accomodate the macro expansion's code.
// To do this, blank lines are inserted. These blank lines however are not
// considered actual lines for the user. So as far as line numbering is
// concerened, these blank lines have the same line number as the line above
// them. These kind of line numbers are called "display line numbers". The
// Parser module and Core module however do not know about that kind of line
// numbering. They use the normal line numbering that assign a sequential line
// number even to macro blank lines. These kind of line numbers are called "raw
// line numbers". QML interface components index their line numbers starting
// from 0 which are called "line indexes".
//
// The following naming conventions are (mostly) used in this file:
// - Display Line Numbers: Values from 1 to m with macro
// blank lines factored out
// (e.g. blank lines have same line number as line above). A variable refering
// to lines not specified to be display or raw is usually type display (i.e.
// display is default).
// - Raw Line Numbers: Values from 1 to n with macro blank
// lines treated as normal lines (therefore n >= m).
// - Line Indexes: Values from 0 to k. Used by QML components and often used for
// positioning.
//
// The functions `convertRawLineNumberToDisplayLineNumber` and
// `convertDisplayLineNumberToRawLineNumber` can be used to translate between
// both types of line numbers.

ScrollView {
  id: scrollView
  verticalScrollBarPolicy: Qt.ScrollBarAsNeeded
  horizontalScrollBarPolicy: Qt.ScrollBarAsNeeded

  Rectangle {
    anchors.fill: parent
    color: Theme.editor.background
  }

  // The Flickable of ScrollView
  Flickable {
    id: container
    focus: false
    anchors.fill: parent

    contentWidth: (textRegion.contentWidth + sidebar.width)*scale.zoom;
    contentHeight: textRegion.contentHeight*scale.zoom;

    // Wrapper item, Flickable can only have one child
    Item {
      id: item
      anchors.fill: parent

      // Scaling transofmation for zooming
      transform: Scale {
        id: scale
        property real zoom: 1
        origin.x: 0;
        origin.y: 0;
        xScale: zoom;
        yScale: zoom;
      }

      HelpToolTip {
        id: _toolTip
        maxWidth: scrollView.viewport.width/2
        maxHeight: scrollView.viewport.height/2
        relativeX: {
          return Math.min(
            scrollView.viewport.width - realWidth - x + container.contentX, 0);
        }
        relativeY: {
          var newY =
            scrollView.viewport.height - realHeight - y + container.contentY;
          return Math.min(newY, textRegion.cursorRectangle.height)
        }

          z: parent.z + 1
        }

        TextRegion {
          id: textRegion
          property real unscaledWidth: {
            return Math.max(
              scrollView.viewport.width - sidebar.width,
              contentWidth
            );
          }
          property real unscaledHeight: {
            return Math.max(scrollView.viewport.height, contentHeight);
          }

          width: (textRegion.unscaledWidth) * scale.zoom
          height: (textRegion.unscaledHeight) * scale.zoom
        }

        FontMetrics {
          id: fontMetrics
          font: textRegion.font
        }

        Sidebar { id: sidebar }

        ZoomMouseArea { }
      }
    }
  }
