/*
* ERASIM Assembler Interpreter
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
import "Snapshots"
import "Output"
import "editor"
import "Memory"
import "RegisterComponent"
import "Input"
import "help"

Item {
  id: holder
  property var usual
  property QtObject current: Rectangle {}

  Component {
    id: snapshots
    SnapshotList { }
  }

  Component {
    id: output
    Output { anchors.fill: parent }
  }

  Component {
    id: register
    RegisterComponent { anchors.fill: parent }
  }

  Component {
    id: memory
    MemoryComponent { anchors.fill: parent }
  }

  Component {
    id: input
    Input { anchors.fill: parent }
  }

  Component {
    id: help
    HelpWindow { anchors.fill: parent }
  }

  function change(component) {
    if (component === "nothing") {
      change(usual);
    } else if (component === "snapshots") {
      var object = snapshots.createObject(holder);
      current.destroy();
      current = object;
    } else if (component === "output") {
      var object = output.createObject(holder);
      current.destroy();
      current = object;
    } else if (component === "register") {
      var object = register.createObject(holder);
      current.destroy();
      current = object;
    } else if (component === "memory") {
      var object = memory.createObject(holder);
      current.destroy();
      current = object;
    } else if (component === "input") {
      var object = input.createObject(holder);
      current.destroy();
      current = object;
    } else if (component === "help") {
      var object = help.createObject(holder);
      current.destroy();
      current = object;
    }
  }
}
