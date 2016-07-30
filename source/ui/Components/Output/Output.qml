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


/*this modul is the contaier for the output-windows*/

import QtQuick 2.0
import QtQuick.Controls 1.4

Item {

    /*Dummys, created by starting the window because of time reasons. They will not be active unless they are choosen*/
    LightStrip{
        id: greenBackground
        anchors.fill: parent
        visible: false
        enabled: false
    }

    BlueRectangle{
        id: blueBackground
        anchors.fill: parent
        visible: false
        enabled: false
    }

    RedRectangle{
        id: redBackground
        anchors.fill: parent
        visible: false
        enabled: false
    }

    /*Drop-Down button*/
    ComboBox{
        width: 150
        model: ["Choose Output","Red Rectangle", "Blue Rectangle", "Light Strip"]

        onCurrentIndexChanged:{
            /*If another line is choosen, an other Output will be set active, al others stay inactive*/
            if(currentIndex==0){
                redBackground.visible=false;
                redBackground.enabled=false;
                blueBackground.visible=false;
                blueBackground.enabled=false;
                greenBackground.visible=false;
                greenBackground.enabled=false;
            }else if(currentIndex==1){
                redBackground.visible=true;
                redBackground.enabled=true;
                blueBackground.visible=false;
                blueBackground.enabled=false;
                greenBackground.visible=false;
                greenBackground.enabled=false;
            }else if(currentIndex==2){
                redBackground.visible=false;
                redBackground.enabled=false;
                blueBackground.visible=true;
                blueBackground.enabled=true;
                greenBackground.visible=false;
                greenBackground.enabled=false;
            }else if(currentIndex==3){
                redBackground.visible=false;
                redBackground.enabled=false;
                blueBackground.visible=false;
                blueBackground.enabled=false;
                greenBackground.visible=true;
                greenBackground.enabled=true;
            }
        }
    }


}
