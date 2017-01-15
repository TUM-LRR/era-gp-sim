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
  // Tell SplitViewItem (i.e. component wrapper) that settings are available to make it display settings icon.
  property bool hasComponentSettings: true

  TreeView {
    id: registerTreeView

    // Set the TreeView's style.
    anchors.fill: parent
    backgroundVisible: false
    alternatingRowColors: false
    headerVisible: false
    selectionMode: SelectionMode.NoSelection
    style: TreeViewStyle {
      transientScrollBars: true
    }

    signal formatAllRegisters(string format)

    // When loading, each register uses the default format if no other
    // cached format is available.
    property var defaultFormat: "Binary"

    onFormatAllRegisters: {
      // Set default format to make newly loaded registers use this formatSelector.
    defaultFormat = format;
    // Clear cache to prevent default format from being overwritten.
    dataTypeFormatCache = ({});
  }

  // Map for associating the currently selected data type format (e.g. bin, hex
  // etc.) with the QModelIndex of the corresponding register. This is required
  // in order to be able to restore the last currently selected format after a
  // delegate item has been delete because it was scrolled out of the visible
  // area. This property cannot be part of the model, as the model can be
  // assigned to multiple TreeViews.
  property var dataTypeFormatCache: ({})

  // Uses the RegisterModel to populate the registers.
  model: registerModel

  rowDelegate: Item { height: Theme.register.height }
  TableViewColumn { title: "Register" }

  itemDelegate: Item {
    id: treeViewItemDelegate

    // Delegates of some well-supported QML-views (such as ListView) have a
    // property for accessing the view the delegate is attached to from a
    // delegate's sub-component via ListView.view. However treeView does not
    // have such a property. In order to still be able to access some of the
    // TreeView's properties, we have to create a property inside the delegate
    // itself, which can then be used from within the delegate's components.
    property TreeView treeView: registerTreeView

    Label {
      id: registerName
      anchors.left: parent.left
      anchors.leftMargin: Theme.register.margin
      anchors.verticalCenter: parent.verticalCenter

      text: model ? model.Title : ""
      font.pixelSize: Theme.register.label.fontSize
      font.weight: Font.DemiBold
      color: Theme.register.label.color
    }

    Loader {
      id: registerContent
      focus: true
      anchors {
        left: registerName.right
        leftMargin: Theme.register.content.margin
        right: formatSelector.left
        rightMargin: Theme.register.content.margin
        verticalCenter: registerName.verticalCenter
      }
      source: {
        if (formatSelector.selection === "Flag") {
          return "FlagRegister.qml";
        } else {
          return "DefaultRegister.qml";
        }
      }
      Keys.onPressed: {
        if (event.key === Qt.Key_Up || event.key === Qt.Key_Left) {
          formatSelector.previousFormat();
        } else if (event.key === Qt.Key_Down || event.key === Qt.Key_Right) {
          formatSelector.nextFormat();
        }
      }
    }

    FormatSelector {
      id: formatSelector
      index: styleData.index
      treeView: registerTreeView
      registerModel: registerTreeView.model
      anchors {
        right: parent.right
        rightMargin: Theme.register.margin
        verticalCenter: registerContent.verticalCenter
      }
    }

    Connections {
      target: registerTreeView
      // When all registers should be formatted, change the format of all
      // registers currently visible (and therefore loaded).
      onFormatAllRegisters: {
        var defaultFormatIndex = indexOfFormat(treeView.defaultFormat);
        if (defaultFormatIndex !== -1) {
          formatSelector.currentIndex = defaultFormatIndex;
        }
      }
    }

    property var formats: ({
      Float: ["Binary", "Hexadecimal"],
      Vector: ["Binary", "Hexadecimal"],
      Flag: ["Flag", "Binary"],
      Link: ["Binary", "Hexadecimal"],
      ProgramCounter: ["Binary", "Hexadecimal"],
      Integer: [
      "Binary", "Hexadecimal", "Unsigned Decimal", "Signed Decimal"
      ]
    })

    // Searches for a given data format string and returns the corresponding
    // index inside the register's associated type's format list.
    function indexOfFormat(format) {
      var type = registerModel.data(styleData.index, 1);
      return type ? formats[type].indexOf(format) : -1;
    }
  }

}

ContextMenu {
  id: contextMenu
  anchors.fill: parent
}
}
