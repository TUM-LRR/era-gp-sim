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
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

import Theme 1.0
import Settings 1.0

Setting {
  text: "Theme"
  description: "The UI and syntax theme. " +
               "Changes apply instantly and automagically."
  bottomAnchor: selector.bottom

  property alias selection: selector.currentText
  property bool differentThanInitially: {
    return selector.currentText !== Settings.theme;
  }

  ComboBox {
    id: selector
    model: Settings.listOfAllThemeNames()
    anchors {
      top: parent.topAnchor
      topMargin: Theme.settings.h2.marginBottom
      horizontalCenter: parent.horizontalCenter
    }
    onCurrentTextChanged: {
      if (selector.currentText !== Theme.current) {
        Theme.load(selector.currentText);
      }
    }
  }
}
