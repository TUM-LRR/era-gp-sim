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
      // Set default format to make newly loaded registers use this format.
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

  itemDelegate: Item {
    id: treeViewItemDelegate
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
    property TreeView attachedTreeView: registerTreeView

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
        right: dataTypeFormatComboBox.left
        rightMargin: Theme.register.content.margin
      }
    }

    // The list of available data formats.
    ComboBox {
      id: dataTypeFormatComboBox
      width: 18
      anchors.verticalCenter: registerContent.verticalCenter
      anchors.right: parent.right
      height: registerContent.height
      property bool shouldCacheFormatIndex: false
      model: {
        // Property bindings are magic; dark, dark magic. So how often this
        // function is called depends on your version of Qt. The problem is,
        // whenever the model is changed, the currentIndex of the ComboBox is
        // reset to 0 (=Binary) Depending on what your version of Qt is, this
        // might happen when you edit the register or just scroll through. So
        // the current format needs to be cached and restored. We only want to
        // cache a current index if it was changed by the user rather than
        // automatically (e.g. through reset). In order to prevent us from
        // caching index 0 after the model was changed, we set
        // dataTypeFormatComboBox.shouldCacheFormatIndex to false which prevents
        // caches until it's set to true again in the onModelChanged-signal.
        dataTypeFormatComboBox.shouldCacheFormatIndex = false;
        // Note: model.Type does not work...
        var type = registerModel.data(styleData.index, 1);
        return formats[type];
      }

      onModelChanged: {
        // Refer to property-binding to dataTypeFormatComboBox.model for further information.
        _tryToRestoreCachedFormatIndex();
        dataTypeFormatComboBox.shouldCacheFormatIndex = true;
      }

      Component.onCompleted: {
        // Refer to property-binding to dataTypeFormatComboBox.model for further information.
        _tryToRestoreCachedFormatIndex();
        dataTypeFormatComboBox.shouldCacheFormatIndex = true;
      }

      onCurrentIndexChanged: {
        // Certain registers require a different content item than a text field.
        // They are loaded when such a data format is selected.
        // For the content to be reloaded correctly, the source component has to be
        // cleared.
        registerContent.sourceComponent = undefined;
        if (currentText == "Flag") {
          registerContent.source = "FlagRegister.qml";
        } else {
          registerContent.source = "DefaultRegister.qml"
        }
        // Cache the new currently selected data type format, but only if the
        // index change was not triggered upon the creation of the combo box
        // (defaults to 0 at creation time) and only if the index is valid
        // (i.e. row and column are positive).
        if (dataTypeFormatComboBox.shouldCacheFormatIndex && styleData.index.row >= 0 && styleData.index.column >= 0) {
          attachedTreeView.dataTypeFormatCache[styleData.index] = currentIndex;
        }
      }

      function _tryToRestoreCachedFormatIndex() {
        // Try to restore a cached selected data type format.
        if (attachedTreeView.dataTypeFormatCache[styleData.index] !== undefined) {
          dataTypeFormatComboBox.currentIndex = attachedTreeView.dataTypeFormatCache[styleData.index];
        } else {    // If no cached format could be restored, load the default format.
          var defaultFormatIndex = indexOfFormat(attachedTreeView.defaultFormat);
          // If the default format is not available for this register's type (e.g. decimal for program counter),
          // load the format at index 0.
          if (defaultFormatIndex !== -1 && defaultFormatIndex !== undefined) {
            dataTypeFormatComboBox.currentIndex = defaultFormatIndex;
          }
        }
      }

      style: ComboBoxStyle {
        // Selection Indicator
        label: Item {
          id: comboBoxSelectionIndicator
          // Triangle Up
          Text {
            id: triangleUp
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: -3
            text: "\u25b2"
            font.pointSize: 7
          }
          // Triangle Down
          Text {
            id: triangleDown
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: triangleUp.bottom
            anchors.topMargin: 2
            text: "\u25bc"
            font.pointSize: 7
          }
        }
        // Remove ComboBox-background
        background: Rectangle {
          visible: false
        }
      }
    }

    Connections {
      target: registerTreeView
      // When all registers should be formatted, change the format of all
      // registers currently visible (and therefore loaded).
      onFormatAllRegisters: {
        var defaultFormatIndex = indexOfFormat(attachedTreeView.defaultFormat);
        if (defaultFormatIndex !== -1) {
          dataTypeFormatComboBox.currentIndex = defaultFormatIndex;
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
      "Binary", "Hexadecimal", "Decimal (Unsigned)", "Decimal (Signed)"
      ]
    })

    // Searches for a given data format string and returns the corresponding index inside the
    // register's associated type's format list.
    function indexOfFormat(format) {
      var type = registerModel.data(styleData.index, 1);
      return formats[type].indexOf(format);
    }
  }

  TableViewColumn { title: "Register" }
}

ContextMenu {
  id: contextMenu
  anchors.fill: parent
}
}
