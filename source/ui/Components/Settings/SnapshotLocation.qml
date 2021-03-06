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
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

import Theme 1.0
import Settings 1.0

Setting {
  id: root
  width: window.width
  text: "Snapshot Location"
  description: "The path at which to load and store snapshots."

  widget: button
  setting: 'snapshotLocation'
  value: button.text

  Connections {
    target: window
    onStore: {
      if (snapshotLocation.hasUnsavedChanges) {
        Settings.snapshotLocationChanged(snapshotLocation.value);
      }
    }
  }

  Button {
    id: button
    anchors {
      top: parent.topAnchor
      topMargin: Theme.settings.h2.marginBottom
      left: parent.left
      right: parent.right
    }

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.PointingHandCursor
      onClicked: fileDialog.open()
    }

    text: Settings.snapshotLocation

    // The tooltip is not bounded by a length, so it can show
    // the full path when the button is not wide enough for it
    tooltip: text

    style: ButtonStyle {
      label: Text {
        elide: Text.ElideRight
        wrapMode: Text.Wrap
        font.pixelSize: Theme.settings.snapshots.button.fontSize
        horizontalAlignment: Text.AlignHCenter
        leftPadding: Theme.settings.snapshots.button.paddingLeft
        maximumLineCount: 1
        rightPadding: Theme.settings.snapshots.button.paddingRight
        text: button.text
      }
      background: Rectangle {
        color: Theme.settings.snapshots.button.background
      }
    }

    FileDialog {
        id: fileDialog
        title: "Select Snapshot Location"
        selectFolder: true
        onAccepted: {
          var location = fileDialog.folder.toString();
          button.text = location.slice("file://".length);
        }
    }
  }
}
