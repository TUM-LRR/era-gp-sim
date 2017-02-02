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

MouseArea {
  width: textRegion.width
  height: textRegion.height

  propagateComposedEvents: true
  scrollGestureEnabled: true
  acceptedButtons: Qt.MiddleButton

  onWheel: {
    if (wheel.modifiers == Qt.ControlModifier) {
      if (wheel.angleDelta.y > 0) {
        parent.addToZoom(0.1);
      } else if (wheel.angleDelta.y < 0) {
        parent.addToZoom(-0.1);
      }

      textRegion.cursorScroll(textRegion.cursorRectangle);
      wheel.accepted = true;
    } else {
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
