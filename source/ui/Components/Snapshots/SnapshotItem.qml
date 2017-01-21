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
import Settings 1.0

Component {
  id: listDelegate
  Item {
    id: item
    property alias text: snapshot.text
    property bool active: listView.currentIndex == model.index

    height: snapshot.height
    width: listView.width

    TextButton {
      id: snapshot
      text: model.modelData
      theme: Theme.snapshots.item
      highlighted: active
      onClicked: {
        guiProject.loadSnapshot(model.modelData);
        listView.currentIndex = model.index;
      }
      anchors {
        right: removeButton.left
        left: parent.left
        bottom: divider.bottom
      }
    }

    SingleSettingDialog {
      id: removalDialog
      text: "Do you want to permanently remove the snapshot?"
      propertyName: 'removeSnapshotsPermanently'
      onDone: removeSnapshot(answerWasYes)
    }

    TextButton {
      id: removeButton
      anchors.bottom: divider.bottom
      text: "\u00D7"
      theme: Theme.snapshots.item.remove
      onClicked: {
        if (removalDialog.alreadyAsked) {
          removeSnapshot(Settings.removeSnapshotsPermanently);
        } else {
          removalDialog.open()
        }
      }
    }

    Divider { id: divider }

    function removeSnapshot(removePermanently) {
      guiProject.removeSnapshot(model.modelData, removePermanently);
    }
  }
}
