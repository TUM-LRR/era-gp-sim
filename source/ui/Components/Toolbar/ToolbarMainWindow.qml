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
  This ile implements the toolbar for the Main Window
 */

import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

ToolBar {
    /*
      The two components are for the buttons, so it can be senn, wether they are pressed
      */
    Component{
        id: styleNotClicked
        ButtonStyle{
            background: Rectangle{
            color: "white"
            anchors.fill: parent
            }
        }
    }

    Component{
        id: styleClicked
        ButtonStyle{
            background: Rectangle{
            color: "lightblue"
            anchors.fill: parent
            }
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
            }
        }

        Item{ width: 10}

        ToolButton{
            id: stop
            enabled: false
            //iconSource: "Icons/StopButtonInactiveWithBorder.svg"
            iconSource: "Icons/StopButtonInactive.svg"
            onClicked: {
                console.info("Stop clicked");
                setInactive();
            }

            /*Functions to enable/disable the Button*/
            function changeActive(){
                if(!enabled){
                    setActive();
                }else{
                    setInactive();
                }
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

        /* The next Buttons should be on the right side*/
        Item{ Layout.fillWidth: true}

        ToolButton{
            id: bin
            style: styleClicked
            /*If there was no text, you could not read the Text*/
            text: "           "
            Text{
                text: "Bin"
                font.bold: true
                anchors.centerIn: parent
                font.pixelSize: 16
            }

            onClicked: {
                console.info("Bin clicked");
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
            text: "          "
            Text{
                text: "Oct"
                font.bold: true
                anchors.centerIn: parent
                font.pixelSize: 16
            }
            onClicked: {
                console.info("Oct clicked");
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
            text: "           "
            Text{
                text: "Dec"
                font.bold: true
                anchors.centerIn: parent
                font.pixelSize: 16
            }
            style: styleNotClicked
            onClicked: {
                console.info("Dec clicked");
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
            text: "           "
            Text{
                text: "Hex"
                font.bold: true
                anchors.centerIn: parent
                font.pixelSize: 16
            }
            style: styleNotClicked
            onClicked: {
                console.info("Hex clicked");
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
