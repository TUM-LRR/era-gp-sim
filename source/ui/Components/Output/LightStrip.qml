import QtQuick 2.0
import QtQuick.Layouts 1.3

import "../Common"

Item {

    property int maxStrips: 10

    NumericUpDown{
        id: numUpDown
        y: 50
    }

    SystemPalette {
        id: systemColorPalette
        colorGroup: SystemPalette.Active
    }

    ListModel {
        id: strips

        ListElement {
            _color: "yellow"
        }
        ListElement {
            _color: "yellow"
        }
        ListElement {
            _color: "yellow"
        }
        ListElement {
            _color: "yellow"
        }
        ListElement {
            _color: "yellow"
        }
        ListElement {
            _color: "white"
        }
        ListElement {
            _color: "white"
        }
        ListElement {
            _color: "white"
        }
    }

    ListView {
        spacing: 8
        width: 180; height: 200
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
