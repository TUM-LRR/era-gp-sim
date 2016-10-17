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
import QtQuick.Layouts 1.2

    /*Main Splitview, for columns*/
SplitView{
    orientation: Qt.Horizontal

    /*Rows*/
    InnerSplitviews{
        Layout.minimumWidth: 10
        width: parent.width/4
        id: splitView1
        faktor: 2
        quotient: 3
        usual1: "snapshots"
        usual2: "output"

    }


    InnerSplitviewsEditor{
        Layout.minimumWidth: 10
        width: parent.width/4
        id: splitView2
        faktor: 2
        quotient: 3
        usual2: "output"
    }

    InnerSplitviews{
        Layout.minimumWidth: 10
        width: parent.width/4
        id: splitView3
        faktor: 1
        quotient: 2
        usual1: "register"
        usual2: "register"
    }


    InnerSplitviews{
        Layout.minimumWidth: 10
        width: parent.width/4
        id: splitView4
        faktor: 1
        quotient: 2
        usual1: "memory"
        usual2: "memory"
    }


    /*Sets the widht at the beginning, because else the columns are too small*/
    onWidthChanged: {
        splitView1.width=(width/10);
        splitView2.width=5*(width/10);
        splitView3.width=2*(width/10);
        splitView4.width=2*(width/10);
    }


}
