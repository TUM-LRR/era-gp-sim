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
import "Snapshots"
import "Output"
import "editor"
import "Memory"
import "Register"

Item {
    id: holder
    /*usual component of the holder*/
    property var usual
    property QtObject actuall: Rectangle{}


    /*All possible Components*/
    Component{
        id: snapshots
        SnapshotList{
        }
    }

    Component{
        id: output
        Output{
            anchors.fill: parent
        }
    }

    Component{
        id: register
        Register{
            anchors.fill: parent
        }
    }

    Component{
        id: memory
        MemoryComponent{
            anchors.fill: parent
        }
    }

    function change(comp){
        if(comp=="nothing"){
            //console.info(usual);
            change(usual);
        }
        else if(comp=="snapshots"){
            var object=snapshots.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
        else if(comp=="output"){
            var object=output.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
        else if(comp=="register"){
            var object=register.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
        else if(comp=="memory"){
            var object=memory.createObject(holder);
            actuall.destroy();
            actuall=object;
        }
        else{
            console.info("Unknowen component");
        }
    }
}
