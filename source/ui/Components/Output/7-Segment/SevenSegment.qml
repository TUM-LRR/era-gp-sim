
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

/*
  a 7-segment digit for displaying numbers:
   _
  |_|
  |_|

*/

import QtQuick 2.0

Item {
    Rectangle {
        id: digit
        x: 100
        y: 50
        width: 100
        height: 200

        // thickness for each segment
        property int thickness: 20
        // background color for the rectangular region around the segments
        property color background_color: "gray"
        // color of the segments
        property color forground_color: "yellow"
        // color of the text inside the segments
        property color text_color: "gray"

        // rectangle for background
        Rectangle {
            id: canvas
            width: parent.width
            height: parent.height - Math.max(digit.thickness, 10) // add some space for the textual representation below
            color: digit.background_color

            /*
              Segments:
              will fit their sizes to the parents dimensions
              displays the index of the digit in a byte that represent the segment's state
             */
            SevenSegment_Segment {
                id: top
                x: 1
                y: digit.thickness / 2
                color: digit.forground_color
                width: digit.thickness
                height: canvas.width - digit.thickness - 2
                transform: Rotation {
                    origin.x: top.width / 2
                    origin.y: 0
                    angle: -90
                }
                Text {
                    id: top_text
                    text: "0"
                    color: digit.text_color
                    font.pixelSize: digit.thickness
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
                y: digit.thickness / 2 + 1
                color: digit.forground_color
                width: digit.thickness
                height: canvas.height / 2 - digit.thickness / 2 - 2
                Text {
                    id: left_top_text
                    text: "5"
                    color: digit.text_color
                    font.pixelSize: digit.thickness
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.fill: parent
                    opacity: 0
                }
            }
            SevenSegment_Segment {
                id: right_top
                x: canvas.width - digit.thickness
                y: digit.thickness / 2 + 1
                color: digit.forground_color
                width: digit.thickness
                height: canvas.height / 2 - digit.thickness / 2 - 2
                Text {
                    id: right_top_text
                    text: "1"
                    color: digit.text_color
                    font.pixelSize: digit.thickness
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.fill: parent
                    opacity: 0
                }
            }
            SevenSegment_Segment {
                id: middle
                x: 1
                y: canvas.height / 2
                color: digit.forground_color
                width: digit.thickness
                height: canvas.width - digit.thickness - 2
                transform: Rotation {
                    origin.x: top.width / 2
                    origin.y: 0
                    angle: -90
                }
                Text {
                    id: middle_text
                    text: "6"
                    color: digit.text_color
                    font.pixelSize: digit.thickness
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
                y: canvas.height / 2 + 1
                color: digit.forground_color
                width: digit.thickness
                height: canvas.height / 2 - digit.thickness / 2 - 2
                Text {
                    id: left_bottom_text
                    text: "4"
                    color: digit.text_color
                    font.pixelSize: digit.thickness
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.fill: parent
                    opacity: 0
                }
            }
            SevenSegment_Segment {
                id: right_bottom
                x: canvas.width - digit.thickness
                y: canvas.height / 2 + 1
                color: digit.forground_color
                width: digit.thickness
                height: canvas.height / 2 - digit.thickness / 2 - 2
                Text {
                    id: right_bottom_text
                    text: "2"
                    color: digit.text_color
                    font.pixelSize: digit.thickness
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.fill: parent
                    opacity: 0
                }
            }
            SevenSegment_Segment {
                id: bottom
                x: 1
                y: canvas.height - digit.thickness / 2
                color: digit.forground_color
                width: digit.thickness
                height: canvas.width - digit.thickness - 2
                transform: Rotation {
                    origin.x: top.width / 2
                    origin.y: 0
                    angle: -90
                }
                Text {
                    id: bottom_text
                    text: "3"
                    color: digit.text_color
                    font.pixelSize: digit.thickness
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

            Text {
                id: binRepresentation
                text: "01001110"
                anchors.top: canvas.bottom
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                font.pointSize: 50
                horizontalAlignment: Text.AlignHCenter
                fontSizeMode: Text.VerticalFit
            }

            // detects mouse hovers and shows the text inside the segments
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

            // fade-in animation for the text inside the digits
            NumberAnimation {
                id: make_text_visible
                targets: [top_text, middle_text, bottom_text, left_top_text, right_top_text, left_bottom_text, right_bottom_text]
                properties: "opacity"
                from: 0
                to: 1
                duration: 100
            }
            // fade-out animation for the text inside the digits
            NumberAnimation {
                id: make_text_invisible
                targets: [top_text, middle_text, bottom_text, left_top_text, right_top_text, left_bottom_text, right_bottom_text]
                properties: "opacity"
                from: 1
                to: 0
                duration: 100
            }
        }

        function setTextVisibility(visible) {
            if (visible) {
                make_text_visible.start()
            } else {
                make_text_invisible.start()
            }
        }
    }
}
