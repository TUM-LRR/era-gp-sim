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
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import "Components/editor"

SplitView {
  // a tag to identify an inner splitview
  property bool isSplitView: true

  // the settings key of this item
  property string settingsKey

  // the default item of this splitview (in addition to the editor)
  property var usual2

  /*heigth of the first component*/
  property int quotient
  property int factor

  SystemPalette {
    id: systemPalette
  }

  handleDelegate: Rectangle {
    width: 2
    height: 2
    color: Qt.darker(systemPalette.window, 1.5)
  }

  orientation: Qt.Vertical
  Editor {
    id: item1
    Layout.minimumHeight: 25
    height: parent.height/2
  }

  SplitviewItem {
    id: item2
    height: parent.height/2
    Layout.minimumHeight: 10
    usual: parent.usual2
  }

  onUsual2Changed: {
    item2.usual=usual2;
  }

  /*Sets heigth at the beginning*/
  Component.onCompleted: {
    item1.height = factor * (height / quotient)-5;
    item2.height = height - item1.height - 5;
  }

  function storeSettings() {
    projectSettings[settingsKey+"-item2"] = item2.currentComponent;
    projectSettings[settingsKey+"-item1-height"] = item1.height;
    projectSettings[settingsKey+"-item2-height"] = item2.height;
  }

  Connections {
    target: projectSettings
    onSettingsLoaded: {
      item2.currentComponent = projectSettings[settingsKey+"-item2"];
      item1.height = projectSettings[settingsKey+"-item1-height"];
      item2.height = projectSettings[settingsKey+"-item2-height"];
    }
  }
}
