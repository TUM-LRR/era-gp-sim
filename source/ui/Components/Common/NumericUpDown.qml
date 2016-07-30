/* C++ Assembler Interpreter
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
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

Item {
    height: 30
    width: 100

    property int _value: 0
    property int _minValue: 0
    property int _maxValue: 10000
    property int _step: 1

    signal valueChanged(int value)
    signal valueBoundariesChanged(int minValue, int maxValue)

    SystemPalette {
        id: systemColorPalette
        colorGroup: SystemPalette.Active
    }

    on_ValueChanged: {
        textField.text = _value.toString()
        valueChanged(_value)
    }
    on_MinValueChanged: {
        //fit value into value boundary given by minValue und maxValue
        if (_value < _minValue)
            _value = _minValue
        valueBoundariesChanged(_minValue, _maxValue)
    }
    on_MaxValueChanged: {
        //fit value into value boundary given by minValue und maxValue
        if (_value > _maxValue)
            _value = _maxValue
        valueBoundariesChanged(_minValue, _maxValue)
    }

    TextField {
        id: textField
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 20

        text: "0"

        onAccepted: {
            var tmp = parseInt(text)

            //check for valid number
            if(!isNaN(tmp))
            {
                //check for range
                if(tmp < _minValue)
                    _value = _minValue
                else if(tmp > _maxValue)
                    _value = _maxValue
                else
                    _value = tmp
            }else //invalid input: change to previous state
                textField.text = _value.toString()

            console.log("memory size input by user: " + _value)
        }

        //enable changing numbers with arrow keys
        Keys.onUpPressed: button_up.mouseArea.clicked(Qt.LeftButton)
        Keys.onDownPressed: button_down.mouseArea.clicked(Qt.LeftButton)
    }
    Rectangle {
        id: button_up
        property alias mouseArea: mouseArea_up
        property alias text: label_up.text

        //layout
        anchors.right: parent.right
        anchors.top: parent.top
        height: parent.height / 2
        width: 20 + 3

        //styling
        border {
            color: "#B9C5D0"
            width: 1
        }
        radius: 1
        smooth: true

        Text {
            id: label_up
            anchors.centerIn: parent
            text: "△"
            bottomPadding: 3
            color: "#808080"
        }

        MouseArea {
            id: mouseArea_up
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                //fit value into value boundary given by minValue und maxValue
                if (_value + _step > _maxValue)
                    _value = _maxValue
                else
                    _value += _step
            }
            //highlight buttons with system colors on mouseover
            onHoveredChanged: {
                if (containsMouse)
                    parent.border.color = systemColorPalette.highlight
                else
                    parent.border.color = "#B9C5D0"
            }
        }
    }
    Rectangle {
        id: button_down
        property alias mouseArea: mouseArea_down
        property alias text: label_down.text

        //layout
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: parent.height / 2
        width: 20 + 3

        //styling
        border {
            color: "#B9C5D0"
            width: 1
        }
        radius: 1
        smooth: true

        Text {
            id: label_down
            anchors.centerIn: parent
            text: "▽"
            bottomPadding: 2
            color: "#808080"
        }

        MouseArea {
            id: mouseArea_down
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                //fit value into value boundary given by minValue und maxValue
                if (_value - _step < _minValue)
                    _value = _minValue
                else
                    _value -= _step
            }
            //highlight buttons with system colors on mouseover
            onHoveredChanged: {
                if (containsMouse)
                    parent.border.color = systemColorPalette.highlight
                else
                    parent.border.color = "#B9C5D0"
            }
        }
    }
}
