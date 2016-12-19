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

/*
  This file implements the toolbar for the Main Window
 */

import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

ToolBar {
    property alias rowLayout: rowLayout

    // hides and disables the toolbar
    function hideToolbar() {
        visible = false;
        enabled = false;
    }

    // makes sure the toolbar is shown and in the proper state
    function showToolbar() {
        visible = true;
        enabled = true;
        if (tabView.getCurrentProjectItem().isRunning) {
            rowLayout.setExecutionState();
        }
        else {
            rowLayout.setStoppedState();
        }
    }

    /*
      The Buttons should have the Systemcolors if they are clicked
      */
    SystemPalette{
        id: systemColors
        colorGroup: SystemPalette.Active
    }

    /*
      The two components are for the buttons, so it can be seen, whether they are pressed
      */
    Component{
        id: styleNotClicked
        ButtonStyle{
            background: Rectangle{
                color: systemColors.button
                //anchors.fill: parent
                implicitWidth: 40
                implicitHeight: 20
            }
        }
    }


    Component{
        id: styleClicked
        ButtonStyle{
            background: Rectangle{
                color: systemColors.button
                border.color: systemColors.highlight
                border.width: 3
                implicitHeight: 20
                implicitWidth: 40
            }
        }
    }

    Component{
        id: tbText
        Text{
            id: textBin
            text: currentText
            font.bold: true
            anchors.centerIn: parent
            font.pixelSize: 16
        }
    }


    /* Showing the Buttons*/
    RowLayout{
        id: rowLayout
        /*There should be no space between the right buttons*/
        spacing: 1
        anchors.fill: parent

        // has to be called when starting the execution to properly set the toolbar state
        function setExecutionState() {
            tabView.getCurrentProjectItem().isRunning = true;
            run.enabled = false;
            runLine.enabled = false;
            runBreakpoint.enabled = false;
            stop.enabled = true;
            stop.iconSource = "Icons/StopButtonActive.svg";
        }

        // has to be called after every execution stops to properly set the toolbar state
        function setStoppedState() {
          run.enabled = true;
          runLine.enabled = true;
          runBreakpoint.enabled = true;
          stop.enabled = false;
          stop.iconSource = "Icons/StopButtonInactive.svg";
        }

        ToolButton{
            id: run
            //iconSource: "RunButtonWithBorder.svg"
            iconSource: "Icons/RunButton.svg"
            onClicked: {
                console.info("Run clicked");
                rowLayout.setExecutionState();
                ui.run(tabView.getCurrentProjectId());
            }
        }

        /*Let space between the buttons*/
        Item{ width: 10}

        ToolButton{
            id: runLine
            //iconSource: "Icons/RunLineButtonWithBorder.svg"
            iconSource: "Icons/RunLineButton.svg"
            onClicked: {
                console.info("runLine clicked");
                rowLayout.setExecutionState();
                ui.runLine(tabView.getCurrentProjectId());
            }
        }

        Item{ width: 10}

        ToolButton{
            id: runBreakpoint
            //iconSource: "Icons/RunBreakpointButtonWithBorder.svg"
            iconSource: "Icons/RunBreakpointButton.svg"
            onClicked: {
                console.info("runBreakpoint clicked");
                rowLayout.setExecutionState();
                ui.runBreakpoint(tabView.getCurrentProjectId());
            }
        }

        Item{ width: 10}

        ToolButton{
            id: stop
            enabled: false
            tooltip: "stop"
            iconSource: "Icons/StopButtonInactive.svg"
            onClicked: {
                console.info("Stop clicked");
                ui.stop(tabView.getCurrentProjectId());
                setInactive();
            }


            function setActive(){
                enabled=true;
                iconSource="Icons/StopButtonActive.svg";
            }

            function setInactive(){
                enabled=false;
                iconSource="Icons/StopButtonInactive.svg";
            }
        }

        ToolButton {
            id: parseButton
            text: "parse"
            onClicked: {
                ui.parse(tabView.getCurrentProjectId());
                console.log("reset " + tabView.getCurrentProjectId());
            }
        }

        ToolButton {
            id: resetButton
            text: "reset"
            onClicked: {
                ui.reset(tabView.getCurrentProjectId());
            }
        }

        /* The Butons should be on the left side*/
        Item{ Layout.fillWidth: true}
    }
}
