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

  InnerSplitviews {
    Layout.minimumWidth: 10
    width: parent.width/4
    id: splitView1
    settingsKey: "splitview-left"
    factor: 2
    quotient: 3
    usual1: "snapshots"
    usual2: "inputoutput"
    defaultTabLower: 4
  }

  InnerSplitviewsEditor {
    Layout.minimumWidth: 10
    width: parent.width/4
    id: splitView2
    settingsKey: "splitview-editor"
    factor: 2
    quotient: 3
    usual2: "inputoutput"
  }

  InnerSplitviews {
    Layout.minimumWidth: 10
    width: parent.width/4
    id: splitView3
    settingsKey: "splitview-middle"
    factor: 1
    quotient: 2
    usual1: "register"
    usual2: "register"
  }

  InnerSplitviews {
    Layout.minimumWidth: 10
    width: parent.width/4
    id: splitView4
    settingsKey: "splitview-right"
    factor: 1
    quotient: 2
    usual1: "memory"
    usual2: "help"
  }

  /*Sets the widht at the beginning, because else the columns are too small*/
  Component.onCompleted: {
    splitView1.width = 3*(width/20);
    splitView2.width = 9*(width/20);
    splitView3.width = 4*(width/20);
    splitView4.width = 4*(width/20);
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
