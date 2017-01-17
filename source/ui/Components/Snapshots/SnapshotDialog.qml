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

import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Theme 1.0

Dialog {
  id: dialog
  title: "Save Snapshot"

  contentItem: Rectangle {
    implicitHeight: Theme.snapshots.dialog.height
    implicitWidth: Theme.snapshots.dialog.width

    TextField {
      id: name
      focus: true
      placeholderText: "Give your snapshot a name"

      anchors {
        top: parent.top
        topMargin: Theme.snapshots.dialog.marginTop
        left: parent.left
        leftMargin: Theme.snapshots.dialog.marginLeft
        right: parent.right
        rightMargin: Theme.snapshots.dialog.marginRight
      }

      style: TextFieldStyle {
        font.pixelSize: Theme.snapshots.dialog.input.fontSize
        background: Rectangle {
          color: Theme.snapshots.dialog.input.background
          border.color: Theme.snapshots.dialog.input.border.color
          border.width: Theme.snapshots.dialog.input.border.width
          radius: Theme.snapshots.dialog.input.radius
        }
      }
      Keys.onPressed: {
        if (event.key === Qt.Key_Return && text.length > 0) {
          save.clicked();
        }
      }
    }

    Button {
      id: save
      enabled: name.text.length > 0
      text: 'Save'
      anchors {
        top: cancel.top
        bottom: cancel.bottom
        right: cancel.left
      }
      onClicked: {
        ui.saveSnapshot(tabView.getCurrentProjectId(), name.text);
        name.text = ''
        dialog.close()
      }
    }

    Button {
      id: cancel
      text: 'Cancel'
      action: Action {
        shortcut: 'Esc'
        onTriggered: dialog.close()
      }
      anchors {
        right: parent.right
        rightMargin: Theme.snapshots.dialog.button.marginRight
        top: name.bottom
        topMargin: Theme.snapshots.dialog.button.marginTop
      }
    }
  }
}
