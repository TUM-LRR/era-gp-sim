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
import QtQuick.Window 2.2

import Theme 1.0
import "Sections"

Window {
  id: window
  width: 500
  height: 500

  title: "Preferences"
  flags: Qt.Dialog
  modality: Qt.ApplicationModal

  property bool hasUnsavedChanges: false

  onClosing: function(event) {
    if (hasUnsavedChanges) {
      askAboutUnsavedChangesDialog.open();
      event.accepted = false;
    }
  }

  SnapshotLocation {
    id: snapshotLocation
    anchors.top: parent.top
    anchors.topMargin: Theme.settings.paddingTop
    onChange: hasUnsavedChanges = true
  }

  Theme {
    id: theme
    anchors.top: snapshotLocation.bottom
    onChange: hasUnsavedChanges = true
  }

  AskAboutUnsavedChangesDialog {
    id: askAboutUnsavedChangesDialog
    onYes: {
      hasUnsavedChanges = false;
      window.close();
    }
  }

  Button {
    text: "Save"
    anchors {
      top: theme.bottom
      horizontalCenter: parent.horizontalCenter
    }
  }
}
