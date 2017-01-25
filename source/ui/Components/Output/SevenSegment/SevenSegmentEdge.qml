
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

import QtQuick 2.6
import QtQuick.Dialogs 1.0

/*
Represents one generic segment for a 7-segment display:

  /\
  ||
  ||    <=========>
  ||
  \/

*/

Item {
    id: segment
    property color color: "yellow"


    height: 50
    width: 0.1 * height

    Rectangle {
        id: ending1
        transform: Rotation {
            origin.x: ending1.width / 2
            origin.y: ending1.height / 2
            angle: 45
        }
        width: segment.width / Math.sqrt(2)
        height: ending1.width
        color: segment.color
        x: 0.5 * (segment.width - ending1.width)
        y: 0.5 * (segment.width - ending1.width)
    }

    Rectangle {
        id: ending2
        transform: Rotation {
            origin.x: ending2.width / 2
            origin.y: ending2.height / 2
            angle: 45
        }
        width: ending1.width
        height: ending1.height
        color: segment.color
        x: 0.5 * (segment.width - ending1.width)
        y: segment.height - segment.width + 0.5 * (segment.width - ending1.width)
    }

    Rectangle {
        id: middle
        width: segment.width
        height: segment.height - segment.width
        color: segment.color
        x: 0
        y: 0.5 * segment.width
    }
}
