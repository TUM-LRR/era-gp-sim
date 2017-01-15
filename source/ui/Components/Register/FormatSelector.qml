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

import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4

import Theme 1.0

ComboBox {
  id: root

  property var index
  property var registerModel
  property TreeView treeView
  property alias selection: root.currentText

  width: Theme.register.selector.width
  height: Theme.register.selector.height
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
    // root.shouldCacheFormatIndex to false which prevents
    // caches until it's set to true again in the onModelChanged-signal.
    root.shouldCacheFormatIndex = false;
    var type = registerModel.data(index, 1);
    return formats[type];
  }

  onModelChanged: {
    // Refer to property-binding to root.model for further
    // information.
    _tryToRestoreCachedFormatIndex();
    root.shouldCacheFormatIndex = true;
  }

  Component.onCompleted: {
    // Refer to property-binding to root.model for further
    // information.
    _tryToRestoreCachedFormatIndex();
    root.shouldCacheFormatIndex = true;
  }

  onCurrentIndexChanged: {
    // Cache the new currently selected data type format, but only if the
    // index change was not triggered upon the creation of the combo box
    // (defaults to 0 at creation time) and only if the index is valid
    // (i.e. row and column are positive).
    if (root.shouldCacheFormatIndex && index.row >= 0 && index.column >= 0) {
      treeView.dataTypeFormatCache[index] = currentIndex;
    }
  }

  function _tryToRestoreCachedFormatIndex() {
    // Try to restore a cached selected data type root.
    if (treeView.dataTypeFormatCache[registerModel.index] !== undefined) {
      root.currentIndex = treeView.dataTypeFormatCache[index];
    } else {    // If no cached format could be restored, load the default root.
      var defaultFormatIndex = indexOfFormat(treeView.defaultFormat);
      // If the default format is not available for this register's type (e.g. decimal for program counter),
      // load the format at index 0.
      if (defaultFormatIndex !== -1 && defaultFormatIndex !== undefined) {
        root.currentIndex = defaultFormatIndex;
      }
    }
  }

  style: ComboBoxStyle {
    background: Rectangle {
      color: Theme.register.selector.background
      radius: Theme.register.selector.radius
    }
    label: Text {
      text: root.currentText ? root.currentText[0] : ""
      color: Theme.register.selector.color
      font.pixelSize: Theme.register.selector.fontSize
      font.weight: Font.DemiBold
      lineHeight: Theme.register.height
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
      font.capitalization: Font.AllUppercase
    }
  }

  property var formats: ({
    Float: ['Binary', 'Hexadecimal'],
    Vector: ['Binary', 'Hexadecimal'],
    Flag: ['Flag', 'Binary'],
    Link: ['Binary', 'Hexadecimal'],
    ProgramCounter: ['Binary', 'Hexadecimal'],
    Integer: [
    'Binary', 'Hexadecimal', 'Unsigned Decimal', 'Signed Decimal'
    ]
  })

  // Searches for a given data format string and returns the corresponding
  // index inside the register's associated type's format list.
  function indexOfFormat(format) {
    var type = registerModel.data(index, 1);
    return type ? formats[type].indexOf(format) : -1;
  }

  function nextFormat() {
    root.currentIndex = (root.currentIndex + 1) % root.count;
  }

  function previousFormat() {
    root.currentIndex -= 1;
    if (root.currentIndex < 0) {
      root.currentIndex = root.count - 1;
    }
  }
}
