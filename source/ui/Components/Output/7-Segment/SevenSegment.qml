
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
        id: digit
        x: 100
        y: 50
        width: 100
        height: 200
        property int thicknes: 20
        property alias background_color: digit.color
        property color forground_color: "yellow"
        property color text_color: "gray"
        background_color: "gray"

        SevenSegment_Segment {
            id: top
            x: 1
            y: digit.thicknes / 2
            color: digit.forground_color
            width: digit.thicknes
            height: digit.width - digit.thicknes - 2
            transform: Rotation {
                origin.x: top.width / 2
                origin.y: 0
                angle: -90
            }
            Text {
                id: top_text
                text: "0"
                color: digit.text_color
                font.pixelSize: digit.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                opacity: 0
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
            y: digit.thicknes / 2 + 1
            color: digit.forground_color
            width: digit.thicknes
            height: digit.height / 2 - digit.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
            Text {
                id: left_top_text
                text: "5"
                color: digit.text_color
                font.pixelSize: digit.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                opacity: 0
            }
        }
        SevenSegment_Segment {
            id: right_top
            x: digit.width - digit.thicknes
            y: digit.thicknes / 2 + 1
            color: digit.forground_color
            width: digit.thicknes
            height: digit.height / 2 - digit.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
            Text {
                id: right_top_text
                text: "1"
                color: digit.text_color
                font.pixelSize: digit.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                opacity: 0
            }
        }
        SevenSegment_Segment {
            id: middle
            x: 1
            y: digit.height / 2
            color: digit.forground_color
            width: digit.thicknes
            height: digit.width - digit.thicknes - 2
            transform: Rotation {
                origin.x: top.width / 2
                origin.y: 0
                angle: -90
            }
            Text {
                id: middle_text
                text: "6"
                color: digit.text_color
                font.pixelSize: digit.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                opacity: 0
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
            y: digit.height / 2 + 1
            color: digit.forground_color
            width: digit.thicknes
            height: digit.height / 2 - digit.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
            Text {
                id: left_bottom_text
                text: "4"
                color: digit.text_color
                font.pixelSize: digit.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                opacity: 0
            }
        }
        SevenSegment_Segment {
            id: right_bottom
            x: digit.width - digit.thicknes
            y: digit.height / 2 + 1
            color: digit.forground_color
            width: digit.thicknes
            height: digit.height / 2 - digit.thicknes / 2 - 2
            //transform: Rotation {origin.x: top.width/2; origin.y: 0; angle: -90}
            Text {
                id: right_bottom_text
                text: "2"
                color: digit.text_color
                font.pixelSize: digit.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                opacity: 0
            }
        }
        SevenSegment_Segment {
            id: bottom
            x: 1
            y: digit.height - digit.thicknes / 2
            color: digit.forground_color
            width: digit.thicknes
            height: digit.width - digit.thicknes - 2
            transform: Rotation {
                origin.x: top.width / 2
                origin.y: 0
                angle: -90
            }
            Text {
                id: bottom_text
                text: "3"
                color: digit.text_color
                font.pixelSize: digit.thicknes
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                opacity: 0
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
                    digit.setTextVisibility(true)
                } else {
                    digit.setTextVisibility(false)
                }
            }
        }

        function setTextVisibility(visible) {
            if (visible) {
                make_text_visible.start()
            } else {
                make_text_invisible.start()
            }
        }

        NumberAnimation {
            id: make_text_visible
            targets: [top_text, middle_text, bottom_text, left_top_text, right_top_text, left_bottom_text, right_bottom_text]
            properties: "opacity"
            from: 0
            to: 1
            duration: 100
        }
        NumberAnimation {
            id: make_text_invisible
            targets: [top_text, middle_text, bottom_text, left_top_text, right_top_text, left_bottom_text, right_bottom_text]
            properties: "opacity"
            from: 1
            to: 0
            duration: 100
        }
    }
}
