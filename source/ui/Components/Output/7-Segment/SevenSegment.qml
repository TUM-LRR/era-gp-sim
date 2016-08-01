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
    Rectangle{
        id: number
        x: 100
        y: 50
        width: 100
        height: 200
        property int thicknes: 20
        property alias background_color: number.color
        property color forground_color: "yellow"
        background_color: "black"

        SevenSegment_Segment {
            id: top
            x: 1
            y: number.thicknes / 2
            color: number.forground_color
            width: number.thicknes
            height: number.width - number.thicknes - 2
            transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: left_top
            x: 0
            y: number.thicknes / 2 + 1
            color: number.forground_color
            width: number.thicknes
            height: number.height /2 - number.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: right_top
            x: number.width - number.thicknes
            y: number.thicknes / 2 + 1
            color: number.forground_color
            width: number.thicknes
            height: number.height /2 - number.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: middle
            x: 1
            y: number.height / 2
            color: number.forground_color
            width: number.thicknes
            height: number.width - number.thicknes - 2
            transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: left_bottom
            x: 0
            y: number.height / 2 + 1
            color: number.forground_color
            width: number.thicknes
            height: number.height /2 - number.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: right_bottom
            x: number.width - number.thicknes
            y: number.height / 2 + 1
            color: number.forground_color
            width: number.thicknes
            height: number.height /2 - number.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }
        SevenSegment_Segment {
            id: bottom
            x: 1
            y: number.height - number.thicknes/2
            color: number.forground_color
            width: number.thicknes
            height: number.width - number.thicknes - 2
            transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
        }

        MouseArea {
            hoverEnabled: true
        }
    }
}
