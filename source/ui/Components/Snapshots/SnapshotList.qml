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
import "../Menubar"

Rectangle {
  anchors.fill: parent
  color: Theme.snapshots.background

  SnapshotHeader { id: header }
  SaveSnapshotDialog { id: saveSnapshotDialog }

  Connections {
    target: snapshotComponent
    onSnapshotsChanged: listView.model = guiProject.getSnapshots()
  }

  SnapshotItem { id: snapshotItem }

  // The list view to display a list of snapshots.
  ListView {
    id: listView
    model: guiProject.getSnapshots();
    currentIndex: -1
    delegate: snapshotItem
    property string currentSnapshot: {
      return listView.currentItem ? listView.currentItem.text : "";
    }
    anchors {
      topMargin: Theme.snapshots.marginTop
      top: header.bottom
      left: parent.left
      bottom: parent.bottom
      right: parent.right
    }
  }

  SaveSnapshotFunctionality {
    id: saveSnapshot
    onNoOverride: saveSnapshotDialog.open();
  }

  Connections {
    target: menubar
    onSaveSnapshotAsRequest: {
      if (projectId === sourceProjectId) {
        saveSnapshotDialog.open();
      }
    }
    onSaveSnapshotRequest: {
      if (projectId === sourceProjectId) {
        saveSnapshot.activate(listView.currentSnapshot, true);
      }
    }
  }
}
