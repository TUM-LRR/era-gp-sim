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


/*this modul is the contaier for the input-windows*/

import QtQuick 2.0
import QtQuick.Controls 1.4

Item {

    /*Dummys, created by starting the window because of time reasons. They will not be active unless they are choosen*/
    ButtonInput{
        id: buttonI
        anchors.fill: parent
        visible: false
        enabled: false
    }

    ClickInput{
        id: clickI
        anchors.fill: parent
        visible: false
        enabled: false
    }

    TextInputSelf{
        id: textI
        anchors.fill: parent
        visible: false
        enabled: false
    }

    /*Drop-Down button*/
    ComboBox{
        width: 150
        model: ["Choose Input","Button", "Click", "Text" ]

        onCurrentIndexChanged:{
            /*If another line is choosen, an other Output will be set active, al others stay inactive*/
            if(currentIndex==0){
                buttonI.visible=false;
                buttonI.enabled=false;
                clickI.visible=false;
                clickI.enabled=false;
                textI.visible=false;
                textI.enabled=false;
            }else if(currentIndex==1){
                buttonI.visible=true;
                buttonI.enabled=true;
                clickI.visible=false;
                clickI.enabled=false;
                textI.visible=false;
                textI.enabled=false;
            }else if(currentIndex==2){
                buttonI.visible=false;
                buttonI.enabled=false;
                clickI.visible=true;
                clickI.enabled=true;
                textI.visible=false;
                textI.enabled=false;
            }else if(currentIndex==3){
                buttonI.visible=false;
                buttonI.enabled=false;
                clickI.visible=false;
                clickI.enabled=false;
                textI.visible=true;
                textI.enabled=true;
            }
        }
    }


}
