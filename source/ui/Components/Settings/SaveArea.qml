/* ERASIM Assembler Interpreter
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
import Settings 1.0

// For ErrorDialog
import "../../"

Rectangle {
  anchors.bottom: parent.bottom
  width: window.width
  height: hasUnsavedChanges ? Theme.settings.saveArea.height : 0
  color: Theme.settings.saveArea.background

  Behavior on height {
    NumberAnimation {
      duration: 200
      easing.type: Easing.OutExpo
    }
  }

  ErrorDialog { id: errorDialog }

  Button {
    id: save
    text: "Save"
    enabled: hasUnsavedChanges
    visible: enabled
    anchors.centerIn: parent
    onClicked: {
      store(); // emit signal

      for (var index = 0; index < sections.withSettings.length; ++index) {
        var section = sections.withSettings[index];
        Settings[section.setting] = section.value;
      }

      var message = Settings.store();
      if (message.length > 0) {
        errorDialog.show(message);
      }
    }
  }
}
