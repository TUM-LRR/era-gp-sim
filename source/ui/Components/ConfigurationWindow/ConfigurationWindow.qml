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

Window {
  width: 500
  height: 500

  title: "Preferences"
  flags: Qt.Dialog
  modality: Qt.ApplicationModal

  onClosing: function(event) {
    console.log(event);
  }

  ///////////////////
  // SNAPSHOT PATH
  ///////////////////

  Rectangle {
    id: snapshotLocation
    anchors {
      top: parent.top
      topMargin: 10
      left: parent.left
      right: parent.right
    }
    height: 100

    Label {
      id: snapshotLabel
      text: "Snapshot Location"
      font.pointSize: 16
      font.weight: Font.DemiBold
      anchors {
        top: parent.top
        left: parent.left
        leftMargin: 10
        right: parent.right
        rightMargin: 10
      }
    }

    Label {
      id: snapshotDescription
      font.pointSize: 13
      anchors {
        top: snapshotLabel.bottom
        topMargin: 8
        left: snapshotLabel.left
        right: snapshotLabel.right
      }
      text: qsTr("The path at which to load and store snapshots.")
      color: "#888888"
    }

    Button {
      id: snapshotButton
      anchors {
        top: snapshotDescription.bottom
        topMargin: 10
        left: parent.left
        right: parent.right
      }

      MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
          snapshotLocationDialog.open()
        }
      }

      text: "/usr/bin/snapshots"

      style: ButtonStyle {
        label: Text {
          elide: Text.ElideRight
          font.pointSize: 14
          horizontalAlignment: Text.AlignHCenter
          leftPadding: 10
          maximumLineCount: 1
          rightPadding: 20
          text: snapshotButton.text // Configuration.snapshotLocation
        }
        background: Rectangle {
          color: "white"
        }
      }

      FileDialog {
          id: snapshotLocationDialog
          title: "Select Snapshot Location"
          selectFolder: true
          onAccepted: {
            var location = snapshotLocationDialog.folder.toString();
            location = location.slice("file://".length);
            snapshotButton.text = location;

            // The tooltip is not bounded by a length, so it can show
            // the full path when the button is not wide enough for it
            snapshotButton.tooltip = location;
          }
      }
    }

    Rectangle {
      anchors {
        top: snapshotButton.bottom
        topMargin: 10
        left: parent.left
        right: parent.right
      }
      height: 1
      color: "#BDBDBD"
    }
  }

  //////////
  // THEME
  //////////

  Rectangle {
    id: theme
    anchors {
      top: snapshotLocation.bottom
      topMargin: 10
      left: parent.left
      right: parent.right
    }
    height: 100

    Label {
      id: themeLabel
      text: "Theme"
      font.pointSize: 16
      font.weight: Font.DemiBold
      anchors {
        top: parent.top
        left: parent.left
        leftMargin: 10
        right: parent.right
        rightMargin: 10
      }
    }

    Label {
      id: themeDescription
      font.pointSize: 13
      anchors {
        top: themeLabel.bottom
        topMargin: 8
        left: themeLabel.left
        right: themeLabel.right
      }
      text: qsTr("The UI and syntax theme. " +
                 "Changes apply instantly and automagically.")
      color: "#888888"
    }

    ComboBox {
      id: themeSelector
      model: ["Darcula", "Solarized Dark", "Solarized Light"]
      anchors {
        top: themeDescription.bottom
        topMargin: 10
        horizontalCenter: parent.horizontalCenter
      }
    }

    Rectangle {
      anchors {
        top: themeSelector.bottom
        topMargin: 10
        left: parent.left
        right: parent.right
      }
      height: 1
      color: "#BDBDBD"
    }
  }

  ////////////////
  // SAVE BUTTON
  ///////////////

  Button {
    id: saveButton
    anchors {
      top: theme.bottom
      horizontalCenter: parent.horizontalCenter
    }

    text: "Save"
  }
}
