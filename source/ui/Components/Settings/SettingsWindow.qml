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
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

import Theme 1.0
import Settings 1.0

Window {
  id: window
  minimumWidth: Theme.settings.minWidth
  minimumHeight: Theme.settings.minHeight

  title: "Preferences"
  flags: Qt.Dialog
  modality: Qt.ApplicationModal

  property bool internalCloseOverride
  property bool hasUnsavedChanges: {
    for (var index = 0; index < sections.withSettings.length; ++index) {
      if (sections.withSettings[index].hasChanged) {
        return true;
      }
    }
    return false;
  }

  signal store();

  AskAboutUnsavedChangesDialog {
    id: askAboutUnsavedChangesDialog

    // Do you really want to quit?"
    onYes: {
      internalCloseOverride = true;
      window.close();

      // The user can load new themes and they will take effect immediately
      // but if he/she does not save, they should not take effect at the end.
      if (themes.hasChanged) {
        Theme.load(Settings.theme);
      }
    }
  }

  onClosing: function(event) {
    if (!internalCloseOverride && hasUnsavedChanges) {
      askAboutUnsavedChangesDialog.open();
      event.accepted = false;
    } else {
      internalCloseOverride = false;
    }
  }

  ScrollView {
    id: scrollView
    anchors {
      left: parent.left
      right: parent.right
      top: parent.top
      bottom: saveArea.top
    }
    Column {
      id: sections
      topPadding: Theme.settings.section.paddingTop
      bottomPadding: Theme.settings.section.paddingBottom

      property var withSettings: {
        var result = [];
        for (var index = 0; index < sections.children.length; ++index) {
          var child = sections.children[index];
          if (child.isSetting) {
            result.push(child);
          }
        }
        return result;
      }

      SnapshotLocation { id: snapshotLocation }

      HorizontalDivider { anchor: snapshotLocation }

      Themes { id: themes }

      HorizontalDivider { anchor: themes }

      CheckboxSetting {
        id: removeSnapshotsPermanently
        text: "Remove Snapshots Permanently"
        description: "If checked, snapshots will be erased " +
                     "permanently when removing them from a project."
        checkboxText: "Yes"
        setting: 'removeSnapshotsPermanently'
      }

      HorizontalDivider { anchor: overrideSnapshots }

      CheckboxSetting {
        id: overrideSnapshots
        text: "Override Snapshots"
        description: "If checked, saving a snapshot will override " +
                    "the one last loaded."
        checkboxText: "Yes"
        setting: 'overrideSnapshots'
      }
    }
  }

  SaveArea { id: saveArea }

}
