
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
Represent one digit of a 7-segment display:
   _
  |_|
  |_|

*/

Rectangle {
    id: digitBackground

    // Thickness of each segment
    property var thickness: 20
    // Background color for the rectangular region around the segments
    property alias backgroundColor: digitBackground.color
    // Color of the segments when active.
    property color digitActiveColor: "yellow"
    // Color of the segments when inactive.
    property color digitInactiveColor: "black"
    // Color of the text inside the segments
    property color textColor: "gray"

    // Determines whether the corresponding segment of the digit is activated.
    property bool segment0Activated: false
    property bool segment1Activated: false
    property bool segment2Activated: false
    property bool segment3Activated: false
    property bool segment4Activated: false
    property bool segment5Activated: false
    property bool segment6Activated: false

    // Sent when one of the digit's segments was pressed.
    signal segmentPressed(var segmentIndex)

    color: "#00000000"

    /*
      Segments:
      Will fit their sizes to the parent's dimensions.
      Displays the index of the digit in a byte that represent the segment's state.
     */

    // Segment 0 (middle-top)
    SevenSegmentEdge {
        id: top
        x: 1
        y: digitBackground.thickness / 2
        color: (segment0Activated) ? digitBackground.digitActiveColor : digitInactiveColor
        width: digitBackground.thickness
        height: digitBackground.width - digitBackground.thickness - 2
        transform: Rotation {
            origin.x: top.width / 2
            origin.y: 0
            angle: -90
        }
        Text {
            id: top_text
            text: "0"
            color: digitBackground.textColor
            font.pixelSize: digitBackground.thickness
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
        MouseArea {
            anchors.fill: parent
            onClicked: segmentPressed(0);
        }
    }

    // Segment 5 (left-top)
    SevenSegmentEdge {
        id: left_top
        x: 0
        y: digitBackground.thickness / 2 + 1
        color: (segment5Activated) ? digitBackground.digitActiveColor : digitInactiveColor
        width: digitBackground.thickness
        height: digitBackground.height / 2 - digitBackground.thickness / 2 - 2
        Text {
            id: left_top_text
            text: "5"
            color: digitBackground.textColor
            font.pixelSize: digitBackground.thickness
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            opacity: 0
        }
        MouseArea {
            anchors.fill: parent
            onClicked: segmentPressed(5);
        }
    }

    // Segment 1 (right-top)
    SevenSegmentEdge {
        id: right_top
        x: digitBackground.width - digitBackground.thickness
        y: digitBackground.thickness / 2 + 1
        color: (segment1Activated) ? digitBackground.digitActiveColor : digitInactiveColor
        width: digitBackground.thickness
        height: digitBackground.height / 2 - digitBackground.thickness / 2 - 2
        Text {
            id: right_top_text
            text: "1"
            color: digitBackground.textColor
            font.pixelSize: digitBackground.thickness
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            opacity: 0
        }
        MouseArea {
            anchors.fill: parent
            onClicked: segmentPressed(1);
        }
    }

    // Segment 6 (middle-middle)
    SevenSegmentEdge {
        id: middle
        x: 1
        y: digitBackground.height / 2
        color: (segment6Activated) ? digitBackground.digitActiveColor : digitInactiveColor
        width: digitBackground.thickness
        height: digitBackground.width - digitBackground.thickness - 2
        transform: Rotation {
            origin.x: top.width / 2
            origin.y: 0
            angle: -90
        }
        Text {
            id: middle_text
            text: "6"
            color: digitBackground.textColor
            font.pixelSize: digitBackground.thickness
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
        MouseArea {
            anchors.fill: parent
            onClicked: segmentPressed(6);
        }
    }

    // Segment 4 (left-bottom)
    SevenSegmentEdge {
        id: left_bottom
        x: 0
        y: digitBackground.height / 2 + 1
        color: (segment4Activated) ? digitBackground.digitActiveColor : digitInactiveColor
        width: digitBackground.thickness
        height: digitBackground.height / 2 - digitBackground.thickness / 2 - 2
        Text {
            id: left_bottom_text
            text: "4"
            color: digitBackground.textColor
            font.pixelSize: digitBackground.thickness
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            opacity: 0
        }
        MouseArea {
            anchors.fill: parent
            onClicked: segmentPressed(4);
        }
    }

    // Segment 2 (right-bottom)
    SevenSegmentEdge {
        id: right_bottom
        x: digitBackground.width - digitBackground.thickness
        y: digitBackground.height / 2 + 1
        color: (segment2Activated) ? digitBackground.digitActiveColor : digitInactiveColor
        width: digitBackground.thickness
        height: digitBackground.height / 2 - digitBackground.thickness / 2 - 2
        Text {
            id: right_bottom_text
            text: "2"
            color: digitBackground.textColor
            font.pixelSize: digitBackground.thickness
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            opacity: 0
        }
        MouseArea {
            anchors.fill: parent
            onClicked: segmentPressed(2);
        }
    }

    // Segment 3 (middle-bottom)
    SevenSegmentEdge {
        id: bottom
        x: 1
        y: digitBackground.height - digitBackground.thickness / 2
        color: (segment3Activated) ? digitBackground.digitActiveColor : digitInactiveColor
        width: digitBackground.thickness
        height: digitBackground.width - digitBackground.thickness - 2
        transform: Rotation {
            origin.x: top.width / 2
            origin.y: 0
            angle: -90
        }
        Text {
            id: bottom_text
            text: "3"
            color: digitBackground.textColor
            font.pixelSize: digitBackground.thickness
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
        MouseArea {
            anchors.fill: parent
            onClicked: segmentPressed(3);
        }
    }


    // Detects mouse hovers and shows the text inside the segments
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        onHoveredChanged: {
            if (containsMouse) {
                setTextVisibility(true)
            } else {
                setTextVisibility(false)
            }
        }
    }

    // Fade-in animation for the text inside the digits
    NumberAnimation {
        id: make_text_visible
        targets: [top_text, middle_text, bottom_text, left_top_text, right_top_text, left_bottom_text, right_bottom_text]
        properties: "opacity"
        from: 0
        to: 1
        duration: 100
    }
    // Fade-out animation for the text inside the digits
    NumberAnimation {
        id: make_text_invisible
        targets: [top_text, middle_text, bottom_text, left_top_text, right_top_text, left_bottom_text, right_bottom_text]
        properties: "opacity"
        from: 1
        to: 0
        duration: 100
    }

    // Starts animation for displaying/hiding the digit's text
    function setTextVisibility(visible) {
        if (visible) {
            make_text_visible.start()
        } else {
            make_text_invisible.start()
        }
    }
}
