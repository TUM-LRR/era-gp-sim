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
import "../Common"

Dialog {
  id: dialog
  property string text
  property string propertyName
  property bool alreadyAsked: Settings[propertyName] !== null
  property alias remember: checkbox.checked

  signal done(bool answerWasYes);

  function processResponse(response) {
    dialog.close();
    if (dialog.remember) {
      Settings[propertyName] = response;
      Settings.store();
    }
    done(response);
  }

  contentItem: Rectangle {
    color: Theme.snapshots.settingDialog.background
    implicitHeight: Theme.snapshots.settingDialog.height
    implicitWidth: Theme.snapshots.settingDialog.width

    Text {
      id: message
      text: dialog.text
      anchors {
        top: parent.top
        topMargin: Theme.snapshots.settingDialog.marginTop
        left: parent.left
        leftMargin: Theme.snapshots.settingDialog.marginLeft
        right: parent.right
        rightMargin: Theme.snapshots.settingDialog.marginRight
      }

      color: Theme.snapshots.settingDialog.text.color
      font.pixelSize: Theme.snapshots.settingDialog.text.fontSize
      font.weight: {
        if (Theme.snapshots.settingDialog.text.fontWeight === 'bold') {
          return Font.DemiBold;
        } else {
          return Font.Normal;
        }
      }
    }

    ThemeableCheckBox {
      id: checkbox
      text: "Remember my answer?"
      theme: Theme.snapshots.settingDialog.checkbox
      anchors {
        top: message.bottom
        topMargin: Theme.snapshots.settingDialog.checkbox.marginTop
        horizontalCenter: parent.horizontalCenter
      }
    }

    Row {
      anchors {
        top: checkbox.bottom
        topMargin: Theme.snapshots.settingDialog.checkbox.marginBottom
        bottom: parent.bottom
        bottomMargin: Theme.snapshots.settingDialog.marginBottom
        horizontalCenter: parent.horizontalCenter
      }

      Button {
        text: "Yes"
        isDefault: true
        onClicked: processResponse(true)
      }

      Button {
        text: "No"
        onClicked: processResponse(false)
      }
    }
  }
}
