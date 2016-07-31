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

import QtQuick 2.0
import QtQuick.Controls 1.4
import "Components"

Item {


    /*default value in the item*/
    property var usual
    property bool isExpanded: false

    onIsExpandedChanged: {
        if(isExpanded==true){
            headerFadeOut.stop()
            headerFadeIn.start()
        }else{
            headerFadeIn.stop()
            headerFadeOut.start()
        }
    }
    SequentialAnimation{
        id: headerFadeIn
        PauseAnimation { duration: 100 }
        PropertyAnimation { target: header; properties: "height"; to: "24"; duration: 100}
        PropertyAnimation { target: componentSelector; properties: "visible"; to: true; duration: 0}
    }
    SequentialAnimation{
        id: headerFadeOut
        PauseAnimation { duration: 100 }
        PropertyAnimation { target: componentSelector; properties: "visible"; to: false; duration: 0}
        PropertyAnimation { target: header; properties: "height"; to: "10"; duration: 100}
    }




    Rectangle {
        id: header
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 10
        color: "gray"

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            propagateComposedEvents: true

            onHoveredChanged: {
                if(containsMouse || componentSelector.hovered || componentSelector.pressed){
                    isExpanded=true
                }else{
                    isExpanded=false
                }
            }
        }

        /*Choose*/
        ComboBox{
            id: componentSelector

            visible: false
            enabled: true
            width: 150
            height: 20
            y: 2
            x: 2
            model: ["Choose Component","Snapshots", "Output", /*"Editor",*/ "Register", "Memory" ]

            onCurrentIndexChanged:{
                if(currentIndex==0){
                    holder.change("nothing");
                }else if(currentIndex==1){
                    holder.change("snapshots");
                }else if(currentIndex==2){
                    holder.change("output");
                }
                else if(currentIndex==3){
                    holder.change("register");
                }
                else{
                    holder.change("memory");
                }
            }
            onPressedChanged: {
                if(pressed)
                    isExpanded=true
                else if(!hovered)
                    isExpanded=false
            }
            onHoveredChanged: {
                if(hovered)
                    isExpanded=true
                else if(!pressed)
                    isExpanded=false
            }
        }
    }

    /*holds the actual component*/
    ComponentHolder{
        id: holder
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        usual: parent.usual
    }
}

