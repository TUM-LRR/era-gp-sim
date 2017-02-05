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

import Theme 1.0

SplitView {
  id: innerSplitview
  // a tag to identify an inner splitview
  property bool isSplitView: true

  // the settings key of this item
  property string settingsKey

  // the default items of this splitview
  property var usual1
  property var usual2

  /*heigth of the first component*/
  property int quotient
  property int factor

  // Allows parent items to set a default tab of each component,
  // if they consist of multiple tabs (i.e. InputOutput).
  property var defaultTabUpper: 0
  property var defaultTabLower: 0

  SystemPalette {
    id: systemPalette
  }

  // height and witdth have to be greater than 1 to achieve correct behaviour on
  // devices with touchscreen
  handleDelegate: Rectangle {
    width: Theme.splitview.handleWidth
    height: Theme.splitview.handleHeight
    color: Theme.splitview.handleColor
  }

  orientation: Qt.Vertical
  SplitviewItem {
    id: item1
    Layout.minimumHeight: 5
    height: parent.height/2
    usual: parent.usual1
    defaultTab: defaultTabUpper
  }

  SplitviewItem {
    id: item2
    height: parent.height/2
    Layout.minimumHeight: 5
    usual: parent.usual2
    defaultTab: defaultTabLower
  }


  onUsual1Changed: {
    item1.usual=usual1;
  }

  onUsual2Changed: {
    item2.usual=usual2;
  }

  /*Sets heigth at the beginning*/
  onHeightChanged: {
    item1.height = factor * (height / quotient) - 5;
    item2.height = height - item1.height - 5;
  }

  function storeSettings() {
    projectSettings[settingsKey+"-item1"] = item1.currentComponent;
    projectSettings[settingsKey+"-item2"] = item2.currentComponent;
    projectSettings[settingsKey+"-item1-height"] = item1.height;
    projectSettings[settingsKey+"-item2-height"] = item2.height;
    projectSettings[settingsKey+"-width"] = width;
  }

  Connections {
    target: projectSettings
    onSettingsLoaded: {
      item1.setCurrentComponent(projectSettings[settingsKey+"-item1"]);
      item2.setCurrentComponent(projectSettings[settingsKey+"-item2"]);
      item1.height = projectSettings[settingsKey+"-item1-height"];
      item2.height = projectSettings[settingsKey+"-item2-height"];
      innerSplitview.width = projectSettings[settingsKey+"-width"];
    }
  }
}
