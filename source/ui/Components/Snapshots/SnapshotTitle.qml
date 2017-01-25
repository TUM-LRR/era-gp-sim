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
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Theme 1.0

  Label {
    height: Theme.snapshots.title.height
    anchors {
      left: parent.left
      top: parent.top
      bottom: parent.bottom
    }

    text: "Snapshots"
    color: Theme.snapshots.title.color
    font.pixelSize: Theme.snapshots.title.fontSize
    verticalAlignment: Text.AlignVCenter;
    horizontalAlignment: {
      if (Theme.snapshots.title.textAlign === 'center') {
        return Text.AlignHCenter;
      } else if (Theme.snapshots.title.textAlign === 'right') {
        return Text.AlignRight;
      } else {
        return Text.AlignLeft;
      }
    }
    font.weight: {
      if (Theme.snapshots.title.fontWeight === 'bold') {
        return Font.DemiBold;
      } else {
        return Font.Normal;
      }
    }
  }
