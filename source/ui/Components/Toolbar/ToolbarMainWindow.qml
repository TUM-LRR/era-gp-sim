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
    /*Knowing the active tab*/
    property TabView tabView

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
        /*There should be no place between the right buttons*/
        spacing: 1
        anchors.fill: parent

        ToolButton{
            id: run
            //iconSource: "RunButtonWithBorder.svg"
            iconSource: "Icons/RunButton.svg"
            onClicked: {
                console.info("Run clicked");
                stop.setActive();
                ui.run(tabView.currentIndex);
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
                stop.setActive();
                ui.runLine(tabView.currentIndex);
            }
        }

        Item{ width: 10}

        ToolButton{
            id: runBreakpoint
            //iconSource: "Icons/RunBreakpointButtonWithBorder.svg"
            iconSource: "Icons/RunBreakpointButton.svg"
            onClicked: {
                console.info("runBreakpoint clicked");
                stop.setActive();
                ui.runBreakpoint(tabView.currentIndex);
            }
        }

        Item{ width: 10}

        ToolButton{
            id: stop
            enabled: false
            tooltip: "stop"
            //iconSource: "Icons/StopButtonInactiveWithBorder.svg"
            iconSource: "Icons/StopButtonInactive.svg"
            onClicked: {
                console.info("Stop clicked");
                ui.stop(tabView.currentIndex);
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

            /*Not implemented yet in the ui*/
            /*Connections{
                target: ui
                onDisableStop: {
                    stop.setInactive();
                    console.info("Geklappt");
                }
            }*/
        }

        ToolButton {
            id: parseButton
            text: "parse"
            onClicked: {
                ui.parse(tabView.currentIndex);
                console.log("reset " + tabView.currentIndex);
            }
        }

        ToolButton {
            id: resetButton
            text: "reset"
            onClicked: {
                ui.reset(tabView.currentIndex);
            }
        }



        /* The next Buttons should be on the right side*/
        Item{ Layout.fillWidth: true}

        ToolButton{
            id: bin
            style: styleClicked
            Loader{
                id: loadB
                property string currentText: "Bin"
                sourceComponent: tbText
                anchors.centerIn: parent
            }

            onClicked: {
                console.info("Bin clicked");
                ui.changeSystem(tabView.currentIndex, loadB.currentText);
                style=styleClicked;
                oct.notClicked();
                dec.notClicked();
                hex.notClicked();

            }

            function notClicked(){
                style=styleNotClicked;
            }

        }


        ToolButton{
            id: oct
            style: styleNotClicked
            Loader{
                id: loadO
                property string currentText: "Oct"
                sourceComponent: tbText
                anchors.centerIn: parent
            }

            onClicked: {
                console.info("Oct clicked");
                ui.changeSystem(tabView.currentIndex, loadO.currentText);
                style=styleClicked;
                bin.notClicked();
                dec.notClicked();
                hex.notClicked();
            }

            function notClicked(){
                style=styleNotClicked;
            }
        }


        ToolButton{
            id: dec
            Loader{
                id: loadD
                property string currentText: "Dec"
                sourceComponent: tbText
                anchors.centerIn: parent
            }
            style: styleNotClicked
            onClicked: {
                console.info("Dec clicked");
                ui.changeSystem(tabView.currentIndex, loadD.currentText);
                style=styleClicked;
                bin.notClicked();
                oct.notClicked();
                hex.notClicked();
            }

            function notClicked(){
                style=styleNotClicked;
            }
        }

        ToolButton{
            id: hex
            Loader{
                id: loadH
                property string currentText: "Bin"
                sourceComponent: tbText
                anchors.centerIn: parent
            }
            style: styleNotClicked
            onClicked: {
                console.info("Hex clicked");
                ui.changeSystem(tabView.currentIndex, loadH.currentText);
                style=styleClicked;
                bin.notClicked();
                oct.notClicked();
                dec.notClicked();
            }

            function notClicked(){
                style=styleNotClicked;
            }
        }
    }
}
