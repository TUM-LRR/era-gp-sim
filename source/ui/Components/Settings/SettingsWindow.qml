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
import Settings 1.0
import "Sections"

Window {
  id: window
  width: 500
  height: 500

  title: "Preferences"
  flags: Qt.Dialog
  modality: Qt.ApplicationModal

  property bool hasUnsavedChanges

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
    initialLocation: Settings.snapshotLocation
    onChange: hasUnsavedChanges = true
    onUnchange: hasUnsavedChanges = false
  }

  Themes {
    id: themes
    anchors.top: snapshotLocation.bottom
    onChange: hasUnsavedChanges = true
    onUnchange: hasUnsavedChanges = false
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
    enabled: hasUnsavedChanges
    anchors {
      top: themes.bottom
      horizontalCenter: parent.horizontalCenter
    }
    onClicked: {
      console.log(themes.selection);
      console.log(snapshotLocation.location);
      // Settings.theme = themes.selection;
      // Settings.snapshotLocation = snapshotLocation.location;
      // Settings.store();
    }
  }
}
