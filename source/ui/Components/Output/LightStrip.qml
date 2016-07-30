import QtQuick 2.0
import QtQuick.Layouts 1.3

import "../Common"

Item {

    property int maxStrips: numUpDown._value
    onMaxStripsChanged: {
        if (maxStrips < 0) {
            maxStrips = 0
        }
        while (strips.count < maxStrips) {
            strips.append({ _color: "white" })
        }
        while (strips.count > maxStrips) {
            strips.remove(strips.count - 1, 1)
        }
    }

    NumericUpDown{
        id: numUpDown
        anchors.right: parent.right
        anchors.rightMargin: 15
        y: 10

        _step: 1
        _value: 8
        _minValue: 0
        _maxValue: 16
    }

    SystemPalette {
        id: systemColorPalette
        colorGroup: SystemPalette.Active
    }

    ListModel {
        id: strips
    }

    ListView {
        spacing: 10
        anchors.fill: parent
        anchors.topMargin: 50
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        orientation: ListView.Horizontal

        model: strips
        delegate: light
    }

    Component {
        id: light
        Rectangle{
            id: rect
            property int _index: index

            color: _color
            height: 100
            width: 20
            border.color: systemColorPalette.highlight
            border.width: 0


            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    if(_color == "yellow")
                        strips.set(_index,{_color: "white"});
                    else
                        strips.set(_index,{_color: "yellow"});
                }
                onEntered: {
                    rect.border.width=1
                }
                onExited: {
                    rect.border.width=0
                }
            }
        }
    }
}
