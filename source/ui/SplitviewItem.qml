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
            header.height=30
            componentSelector.visible=true
        }else{
            header.height=10
            componentSelector.visible=false
        }
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
                if(containsMouse || componentSelector.pressed || componentSelector.hovered){
                    isExpanded=true
                }else{
                    isExpanded=false
                }
            }

            onClicked: {
                mouse.accepted=false
            }
        }

        /*Choose*/
        ComboBox{
            id: componentSelector

            visible: false
            enabled: true
            width: 150
            height: 20
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
                if(!pressed) {
                    isExpanded=false
                }
            }

            /*onPressedChanged: {
                if(pressed==true){mouseArea.pressed=true;}
                else{mouseArea.pressed=false;}
            }*/

        }


    }

    /*Space befot the combobox*/
//    Rectangle{
//        id: space
//        height: 10
//        width: 150
//        //visible: false
//        color: "red"
//    }





//    }




//    /*Space befor the component*/
//    Rectangle{
//        id: rect
//        anchors.top: parent.top
//        width: 150
//        height: 10
//        //visible: false
//        color:  "blue"
//    }

    /*holds the actual component*/
    ComponentHolder{
        id: holder
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        usual: parent.usual
    }

 /*   MouseArea{
        id: mouseArea
        anchors.top: parent.top
        height: 10
        anchors.left:  parent.left
        anchors.right: parent.right
        propagateComposedEvents: true
        property bool pressed: false
        onEntered: {
            //console.info("MouseArea entered");
            box.enabled=true;
            box.visible=true;
            //console.info(box.enabled);;
            rect.anchors.top=box.bottom;
        }
        onExited: {
            if(!pressed){
            //console.info("exited");
                box.visible=false;
                box.enabled=false;
                rect.anchors.top=parent.top;
            }
        }


        hoverEnabled: true
    }*/

}

