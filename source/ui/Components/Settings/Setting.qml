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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.6
import QtQuick.Controls 1.4

import Theme 1.0
import Settings 1.0
import "../../../Js/ThemeUtility.js" as ThemeUtility

Rectangle {
  width: window.width
  height: Theme.settings.section.height

  // 'Public' properties
  property alias text: label.text
  property alias description: description.text
  property alias topAnchor: description.bottom
  property bool isSetting: true // A 'tag' to identify a Setting instance

  property var value
  property string setting
  property var widget

  property bool hasChanged: numberOfChanges > 0 && Settings[setting] !== value

  // 'Private properties'

  // All properties are 'changed' when they are
  // initialized, so we offset this count.
  property int numberOfChanges: -1

  // This rules out changes for null, undefined and the empty string, but
  // ensures changing a boolean to false does indeed constitute a change
  onValueChanged: if (value || value === false) numberOfChanges += 1;

  property var dynamic: ({ theme: ThemeUtility.dynamicThemeFactory(
      Theme.settings,
      hasChanged,
      'changed'
  )})

  Label {
    id: label
    font.pixelSize: Theme.settings.h1.fontSize
    font.weight: Font.DemiBold
    wrapMode: Text.Wrap
    color: Theme.settings.h1.color
    horizontalAlignment: Text.AlignHCenter
    anchors {
      top: parent.top
      horizontalCenter: parent.horizontalCenter
    }
  }

  Rectangle {
    anchors {
      left: label.right
      leftMargin: Theme.settings.status.margin
      verticalCenter: label.verticalCenter
    }
    height: Theme.settings.status.height
    width: Theme.settings.status.width
    radius: Theme.settings.status.radius
    border.color: dynamic.theme('status.border.color')
    border.width: dynamic.theme('status.border.width')
    color: dynamic.theme('status.background')
  }

  Label {
    id: description
    font.pixelSize: Theme.settings.h2.fontSize
    wrapMode: Text.Wrap
    horizontalAlignment: Text.Justify
    color: Theme.settings.h2.color
    anchors {
      top: label.bottom
      topMargin: Theme.settings.h2.marginTop
      left: parent.left
      leftMargin: Theme.settings.h2.marginLeft
      right: parent.right
      rightMargin: Theme.settings.h2.marginRight
    }
  }
}
