/* C++ Assembler Interpreter
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

import "../Common/"

Rectangle {
  id: pixelDisplayError
  radius: 3
  visible: false

  Image {
    id: errorIcon
    anchors.fill: parent

    source: "../editor/Issue Icons/Error Icon.png"
  }

  ToolTip {
    id: errorTooltip
    anchors.fill: parent
  }

  Connections {
    target: pixeldisplaypainteditemid

    onPixelDisplayError: {
      errorTooltip.text = errorMessage;
      pixelDisplayError.visible = true;
      console.log("An error occured in the pixel display");
    }

    onPixelDisplayErrorResolved: {
      pixelDisplayError.visible = false;
      console.log("error resolved");
    }
  }

}
