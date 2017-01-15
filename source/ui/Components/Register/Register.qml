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
* along with this program. If not, see http://www.gnu.org/licenses/.*/

import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4

import Theme 1.0

Item {
  property var model
  property var treeView

  anchors {
    left: parent.left
    leftMargin: Theme.register.margin
    right: parent.right
    rightMargin: Theme.register.margin
  }

  // Delegates of some well-supported QML-views (such as ListView) have a
  // property for accessing the view the delegate is attached to from a
  // delegate's sub-component via ListView.view. However treeView does not
  // have such a property. In order to still be able to access some of the
  // TreeView's properties, we have to create a property inside the delegate
  // itself, which can then be used from within the delegate's components.
  property TreeView attachedTreeView: treeView

  Label {
    id: registerName
    anchors.left: parent.left
    anchors.verticalCenter: registerContent.verticalCenter

    text: model ? model.Title : ""
    font.pixelSize: Theme.register.label.fontSize
    font.weight: Font.DemiBold
    color: Theme.register.label.color
  }

  Loader {
    id: registerContent
    anchors {
      verticalCenter: registerName.verticalCenter
      left: registerName.right
      leftMargin: Theme.register.content.margin
      right: format.left
      rightMargin: Theme.register.content.margin
    }
  }

  FormatSelector {
    id: format
    anchors.verticalCenter: registerContent.verticalCenter
    anchors.right: parent.right
  }

  // Connections {
  //   target: treeView
  //   // When all registers should be formatted, change the format of all
  //   // registers currently visible (and therefore loaded).
  //   onFormatAllRegisters: {
  //     var defaultFormatIndex = indexOfFormat(attachedTreeView.defaultFormat);
  //     if (defaultFormatIndex !== -1) {
  //       format.currentIndex = defaultFormatIndex;
  //     }
  //   }
  // }
}
