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
  id: registerComponent

  // Tell SplitViewItem (i.e. component wrapper) that settings are
  // available to make it display settings icon.
  property bool hasComponentSettings: true

  // Called by SplitViewItem.qml (i.e. component wrapper) when component settings icon was pressed.
  signal settingsButtonPressed


  ContextMenu {
    id: contextMenu
    anchors.fill: parent
  }

  TreeView {
    id: registerTreeView

    // Set the TreeView's style.
    anchors.fill: parent
    alternatingRowColors: false
    headerVisible: false
    selectionMode: SelectionMode.NoSelection
    style: TreeViewStyle {
      backgroundColor: Theme.register.background
      transientScrollBars: Theme.register.overflow === 'auto'
    }

    signal formatAllRegisters(string format)

    // When loading, each register uses the default format if no other
    // cached format is available.
    property var defaultFormat: "Signed Decimal"

    // Map for associating the currently selected data type format (e.g. bin,
    // hex etc.) with the QModelIndex of the corresponding register. This is
    // required in order to be able to restore the last currently selected
    // format after a delegate item has been delete because it was scrolled out
    // of the visible area. This property cannot be part of the model, as the
    // model can be assigned to multiple TreeViews.
    property var dataTypeFormatCache: ({ })

    TableViewColumn { title: "Register" }
    model: registerModel
    rowDelegate: Item { height: Theme.register.height }

    itemDelegate: Register { }

    onFormatAllRegisters: {
      // Set default format to make newly loaded
      // registers use this formatSelector.
      defaultFormat = format;
      // Clear cache to prevent default format from being overwritten.
      dataTypeFormatCache = { };
    }
  }
}
