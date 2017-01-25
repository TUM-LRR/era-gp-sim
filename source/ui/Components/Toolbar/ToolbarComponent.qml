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
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import Theme 1.0

ToolBar {
  property alias rowLayout: rowLayout
  visible: enabled

  property bool running
  onRunningChanged: tabView.getCurrentProjectItem().running = running

  style: ToolBarStyle {
    background: Rectangle {
      color: Theme.toolbar.background
    }
  }

  RowLayout {
    id: rowLayout
    spacing: Theme.toolbar.margin
    anchors.fill: parent

    ToolbarButton {
      enabled: !running
      icon: "Icons/Run.svg"
      tooltip: "Execute the whole program"
      onClicked: {
        running = true;
        ui.run(tabView.getCurrentProjectId());
      }
    }

    ToolbarButton {
      enabled: !running
      icon: "Icons/Step.svg"
      tooltip: "Execute the next line"
      onClicked: {
        running = true;
        ui.runLine(tabView.getCurrentProjectId());
      }
    }

    ToolbarButton {
      enabled: !running
      icon: "Icons/RunToBreakpoint.svg"
      tooltip: "Execute up to next breakpoint"
      onClicked: {
        running = true;
        ui.runBreakpoint(tabView.getCurrentProjectId());
      }
    }

    ToolbarButton {
      enabled: running
      tooltip: "Stop execution"
      icon: "Icons/Stop.svg"
      onClicked: {
        running = false;
        ui.stop(tabView.getCurrentProjectId());
      }
    }

    ToolbarButton {
      tooltip: "Reparse code"
      icon: "Icons/Parse.svg"
      onClicked: ui.parse(tabView.getCurrentProjectId());
    }

    ToolbarButton {
      tooltip: "Reset registers and memory"
      icon: "Icons/Clear.svg"
      onClicked: ui.reset(tabView.getCurrentProjectId());
    }

    // Use up the remaining space to the right
    Item { Layout.fillWidth: true }
  }
}
