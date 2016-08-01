
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
    Rectangle {
        id: number
        x: 100
        y: 50
        width: 100
        height: 200
        property int thicknes: 20
        property alias background_color: number.color
        property color forground_color: "yellow"
        property color text_color: "gray"
        background_color: "gray"

        SevenSegment_Segment {
            id: top
            x: 1
            y: number.thicknes / 2
            color: number.forground_color
            width: number.thicknes
            height: number.width - number.thicknes - 2
            transform: Rotation {
                origin.x: top.width / 2
                origin.y: 0
                angle: -90
            }
            Text {
                id: top_text
                text: "0"
                color: text_color
                font.pixelSize: number.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                visible: false
                transform: Rotation {
                    origin.x: top.width / 2
                    origin.y: top.height / 2
                    angle: 90
                }
            }
        }
        SevenSegment_Segment {
            id: left_top
            x: 0
            y: number.thicknes / 2 + 1
            color: number.forground_color
            width: number.thicknes
            height: number.height / 2 - number.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
            Text {
                id: left_top_text
                text: "5"
                color: text_color
                font.pixelSize: number.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                visible: false
            }
        }
        SevenSegment_Segment {
            id: right_top
            x: number.width - number.thicknes
            y: number.thicknes / 2 + 1
            color: number.forground_color
            width: number.thicknes
            height: number.height / 2 - number.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
            Text {
                id: right_top_text
                text: "1"
                color: text_color
                font.pixelSize: number.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                visible: false
            }
        }
        SevenSegment_Segment {
            id: middle
            x: 1
            y: number.height / 2
            color: number.forground_color
            width: number.thicknes
            height: number.width - number.thicknes - 2
            transform: Rotation {
                origin.x: top.width / 2
                origin.y: 0
                angle: -90
            }
            Text {
                id: middle_text
                text: "6"
                color: text_color
                font.pixelSize: number.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                visible: false
                transform: Rotation {
                    origin.x: middle.width / 2
                    origin.y: middle.height / 2
                    angle: 90
                }
            }
        }
        SevenSegment_Segment {
            id: left_bottom
            x: 0
            y: number.height / 2 + 1
            color: number.forground_color
            width: number.thicknes
            height: number.height / 2 - number.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
            Text {
                id: left_bottom_text
                text: "4"
                color: text_color
                font.pixelSize: number.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                visible: false
            }
        }
        SevenSegment_Segment {
            id: right_bottom
            x: number.width - number.thicknes
            y: number.height / 2 + 1
            color: number.forground_color
            width: number.thicknes
            height: number.height / 2 - number.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
            Text {
                id: right_bottom_text
                text: "2"
                color: text_color
                font.pixelSize: number.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                visible: false
            }
        }
        SevenSegment_Segment {
            id: bottom
            x: 1
            y: number.height - number.thicknes / 2
            color: number.forground_color
            width: number.thicknes
            height: number.width - number.thicknes - 2
            transform: Rotation {
                origin.x: top.width / 2
                origin.y: 0
                angle: -90
            }
            Text {
                id: bottom_text
                text: "3"
                color: text_color
                font.pixelSize: number.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                visible: false
                transform: Rotation {
                    origin.x: bottom.width / 2
                    origin.y: bottom.height / 2
                    angle: 90
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onHoveredChanged: {
                if (containsMouse) {
                    number.setTextVisibility(true)
                } else {
                    number.setTextVisibility(false)
                }
            }
        }

        function setTextVisibility(visible) {
            if (visible) {
                top_text.visible = true
                middle_text.visible = true
                bottom_text.visible = true
                left_top_text.visible = true
                right_top_text.visible = true
                left_bottom_text.visible = true
                right_bottom_text.visible = true
            } else {
                top_text.visible = false
                middle_text.visible = false
                bottom_text.visible = false
                left_top_text.visible = false
                right_top_text.visible = false
                left_bottom_text.visible = false
                right_bottom_text.visible = false
            }
        }
    }
}
