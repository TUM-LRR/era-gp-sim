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

// This file implements the toolbar for the Main Window

import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

ToolBar {
  property alias rowLayout: rowLayout
  visible: enabled

  // The Buttons should have the Systemcolors if they are clicked.
  SystemPalette {
    id: systemColors
    colorGroup: SystemPalette.Active
  }

  // The two components are for the buttons, so it can be
  // seen, whether they are pressed.
  Component {
    id: styleNotClicked
    ButtonStyle {
      background: Rectangle {
        color: systemColors.button
        implicitWidth: 40
        implicitHeight: 20
      }
    }
  }

  Component {
    id: styleClicked
    ButtonStyle {
      background: Rectangle {
        color: systemColors.button
        border.color: systemColors.highlight
        border.width: 3
        implicitHeight: 20
        implicitWidth: 40
      }
    }
  }

  Component {
    Text {
      id: textBin
      text: currentText
      font.bold: true
      anchors.centerIn: parent
      font.pixelSize: 16
    }
  }

  property bool running
  onRunningChanged: tabView.getCurrentProjectItem().running = running;

  RowLayout {
    id: rowLayout
    // There should be no space between the right buttons.
    spacing: 1
    anchors.fill: parent


    ToolButton {
      id: run
      enabled: !running
      iconSource: "Icons/Run.svg"
      tooltip: "Execute the whole program"
      onClicked: {
        running = true;
        ui.run(tabView.getCurrentProjectId());
      }
    }

    Item { width: 10 }

    ToolButton {
      id: runLine
      enabled: !running
      iconSource: "Icons/Step.svg"
      tooltip: "Execute the next line"
      onClicked: {
        running = true;
        ui.runLine(tabView.getCurrentProjectId());
      }
    }

    Item { width: 10 }

    ToolButton {
      id: runBreakpoint
      enabled: !running
      tooltip: "Execute up to next breakpoint"
      iconSource: "Icons/RunToBreakpoint.svg"
      onClicked: {
        running = true;
        ui.runBreakpoint(tabView.getCurrentProjectId());
      }
    }

    Item { width: 10 }

    ToolButton {
      id: stop
      enabled: running
      tooltip: "Stop execution"
      iconSource: "Icons/Stop.svg"
      onClicked: {
        running = false;
        ui.stop(tabView.getCurrentProjectId());
      }
    }

    ToolButton {
      id: parseButton
      tooltip: "Reparse the code"
      iconSource: "Icons/Parse.svg"
      onClicked: ui.parse(tabView.getCurrentProjectId());
    }

    ToolButton {
      id: resetButton
      tooltip: "Reset registers and memory"
      iconSource: "Icons/Clear.svg"
      onClicked: {
        stop.clicked();
        ui.reset(tabView.getCurrentProjectId());
      }
    }

    // The Butons should be on the left side
    Item { Layout.fillWidth: true }
  }
}
