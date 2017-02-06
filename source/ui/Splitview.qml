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

SplitView {
  id: splitviewRoot
  orientation: Qt.Horizontal

  // Height and width are still 0 when the component is completed.
  // Because of this, the inner splitviews have to be resized when the size
  // changes for the first time. Somehow bindings don't work here.
  property bool widthInitialized: false

  InnerSplitviews {
    id: splitView1
    Layout.minimumWidth: 5
    Layout.fillWidth: false
    settingsKey: "splitview-left"
    factor: 2
    quotient: 3
    usual1: "snapshots"
    usual2: "inputoutput"
    defaultTabLower: 4
  }

  InnerSplitviewsEditor {
    id: splitView2
    Layout.minimumWidth: 5
    Layout.fillWidth: false
    implicitWidth: width
    settingsKey: "splitview-editor"
    factor: 2
    quotient: 3
    usual2: "inputoutput"
  }

  InnerSplitviews {
    id: splitView3
    Layout.minimumWidth: 5
    Layout.fillWidth: false
    settingsKey: "splitview-middle"
    factor: 1
    quotient: 2
    usual1: "register"
    usual2: "register"
  }

  InnerSplitviews {
    id: splitView4
    Layout.minimumWidth: 5
    Layout.fillWidth: true
    settingsKey: "splitview-right"
    factor: 1
    quotient: 2
    usual1: "memory"
    usual2: "help"
  }

  onWidthChanged: {
    splitView1.width = 4 * (splitviewRoot.width / 20);
    splitView2.width = 9 * (splitviewRoot.width / 20);
    splitView3.width = 4 * (splitviewRoot.width / 20);
    splitView4.width = 4 * (splitviewRoot.width / 20);
  }

  function collectSettings() {
    for (var index = 0; index < splitviewRoot.children.length; ++index) {
      var child = splitviewRoot.children[index];
      // The children of the splitview are not the real objects, there is a nested structure.
      for (var innerIndex = 0; innerIndex < child.children.length; ++innerIndex) {
        var innerChild = child.children[innerIndex];
        if (innerChild.isSplitView) {
          innerChild.storeSettings();
        }
      }
    }
  }

}
