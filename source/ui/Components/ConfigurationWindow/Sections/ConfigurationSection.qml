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
import QtQuick.Controls 1.4

import Theme 1.0

Rectangle {
  height: Theme.configuration.section.height
  anchors {
    top: parent.top
    left: parent.left
    right: parent.right
  }

  property alias text: label.text
  property alias description: description.text
  property alias topAnchor: description.bottom
  property var bottomAnchor

  Label {
    id: label
    font.pixelSize: Theme.configuration.h1.fontSize
    font.weight: Font.DemiBold
    color: Theme.configuration.h1.color
    anchors {
      top: parent.top
      left: parent.left
      leftMargin: Theme.configuration.h1.padding
      right: parent.right
      rightMargin: Theme.configuration.h1.padding
    }
  }

  Label {
    id: description
    font.pixelSize: Theme.configuration.h2.fontSize
    anchors {
      top: label.bottom
      topMargin: Theme.configuration.h2.marginTop
      left: label.left
      right: label.right
    }
    color: Theme.configuration.h2.color
  }

  Rectangle {
    id: rule
    anchors {
      top: bottomAnchor
      topMargin: Theme.configuration.section.paddingBottom
      left: parent.left
      right: parent.right
    }
    height: Theme.configuration.hr.height
    color: Theme.configuration.hr.color
  }
}
