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


Item {
    Item{
        x: 100
        y: 50
        id: number
        SevenSegment_Segment {
            id: top
            x: 1
            y: 0
            width: 20
            height: 100
            transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: left_top
            x: 0
            y: 1
            width: 20
            height: 100
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: right_top
            x: 102
            y: 1
            width: 20
            height: 100
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: middle
            x: 1
            y: 102
            width: 20
            height: 100
            transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: left_bottom
            x: 0
            y: 102
            width: 20
            height: 100
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: right_bottom
            x: 102
            y: 102
            width: 20
            height: 100
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: bottom
            x: 1
            y: 204
            width: 20
            height: 100
            transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
    }
}
