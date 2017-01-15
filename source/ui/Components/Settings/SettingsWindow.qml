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
import "../../"

Window {
  id: window
  width: 500
  height: 500

  title: "Preferences"
  flags: Qt.Dialog
  modality: Qt.ApplicationModal

  property bool hasUnsavedChanges: {
    return snapshotLocation.differentThanInitially ||
           themes.differentThanInitially;
  }

  signal store();

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
    Connections {
      target: window
      onStore: {
        if (snapshotLocation.differentThanInitially) {
          Settings.snapshotLocationChanged(snapshotLocation.location);
        }
      }
    }
  }

  Themes {
    id: themes
    anchors.top: snapshotLocation.bottom
  }

  AskAboutUnsavedChangesDialog {
    id: askAboutUnsavedChangesDialog
    onYes: {
      hasUnsavedChanges = false;
      window.close();

      // The user can load new themes and they will take effect immediately
      // but if he/she does not save, they should not take effect at the end.
      if (themes.differentThanInitially) {
        Theme.load(Settings.theme);
      }
    }
  }

  ErrorDialog { id: errorDialog }

  Button {
    text: "Save"
    enabled: hasUnsavedChanges
    anchors {
      top: themes.bottom
      horizontalCenter: parent.horizontalCenter
    }
    onClicked: {
      store();

      Settings.theme = themes.selection;
      Settings.snapshotLocation = snapshotLocation.location;

      var message = Settings.store();
      if (message.length > 0) {
        errorDialog.show(message);
      } else {
        hasUnsavedChanges = false;
      }

    }
  }
}
