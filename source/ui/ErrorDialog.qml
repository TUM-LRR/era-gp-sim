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
import QtQuick.Dialogs 1.2


MessageDialog {
  title: "Error"
  // Poor man's margin. Cannot style a message dialog ... :(
  text: "Error                                                          "
  standardButtons: StandardButton.Ok
  icon: StandardIcon.Critical
  onAccepted: close()
  Component.onCompleted: {
    if (errorMessageFromStartup) {
      informativeText = errorMessageFromStartup;
      open();
    }
  }

  function show(text) {
    this.text = text;
    open();
  }
}
