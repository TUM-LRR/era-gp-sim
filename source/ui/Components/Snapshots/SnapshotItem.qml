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
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Theme 1.0

Component {
  id: listDelegate
  Item {
    property alias text: textItem.text
    height: textItem.height
    width: listView.width

    Text {
      id: textItem
      anchors.left: parent.left
      anchors.leftMargin: 3
      anchors.verticalCenter: parent.verticalCenter
      text: model.modelData
      font.bold: true
    }

    Button {
      id: loadButton
      anchors.right: deleteButton.left
      anchors.verticalCenter: parent.verticalCenter
      height: textLoad.height + 2
      width: textLoad.width + 5
      visible: false
      onClicked: guiProject.loadSnapshot(model.modelData);
      Text {
        id: textLoad
        text: "Load"
        color: "green"
        anchors.centerIn: parent
      }
    }

    Button {
      id: deleteButton
      anchors.right: parent.right
      anchors.verticalCenter: parent.verticalCenter
      height: textDelete.height + 2
      width: textDelete.width + 5
      visible: false
      onClicked: guiProject.removeSnapshot(model.modelData);
      Text {
        id: textDelete
        text: "Delete"
        color: "red"
        anchors.centerIn: parent
      }
    }
  }
}
