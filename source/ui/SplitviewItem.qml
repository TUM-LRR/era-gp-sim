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

    /*Space befot the combobox*/
    Rectangle{
        id: space
        height: 10
        width: 150
        visible: false
    }


    /*Choose*/
    ComboBox{
        id: box
        visible: true
        enabled: true
        anchors.top: space.bottom
        width: 150
        height: 20
        model: ["Choose Component","Snapshots", "Output", "Editor", "Register", "Memory" ]

        onCurrentIndexChanged:{
            if(currentIndex==0){
                holder.change("nothing");
            }else if(currentIndex==1){
                holder.change("snapshots");
            }else if(currentIndex==2){
                holder.change("output");
            }
            else if(currentIndex==3){
                holder.change("editor");
            }
            else if(currentIndex==4){
                holder.change("register");
            }
            else{
                holder.change("memory");
            }

        }
    }

    /*Space befor the component*/
    Rectangle{
        id: rect
        anchors.top: box.bottom
        width: 150
        height: 10
        visible: false
    }

    /*holds the actual component*/
    ComponentHolder{
        id: holder
        anchors.top: rect.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        usual: parent.usual
    }





}

