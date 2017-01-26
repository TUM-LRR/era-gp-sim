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
