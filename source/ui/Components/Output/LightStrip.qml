import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0

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

    Rectangle{
        color: "#C7C7C7"
        y: 50
        height: 110
        anchors.left: parent.left
        anchors.right: parent.right

        ListView {
            spacing: 10
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            anchors.topMargin: 5
            anchors.bottomMargin: 5

            orientation: ListView.Horizontal
            interactive: false

            model: strips
            delegate: light
        }
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
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    if(mouse.button === Qt.RightButton){
                        var newObject = Qt.createQmlObject('import QtQuick 2.2
                                                            import QtQuick.Dialogs 1.0

                                                            ColorDialog {
                                                                id: colorDialog
                                                                width: 300
                                                                height: 200
                                                                title: "Please choose a color"
                                                                currentColor: rect.color
                                                                onAccepted: {
                                                                    strips.set(rect._index,{_color: colorDialog.color.toString()});
                                                                    colorDialog.close();
                                                                }
                                                                onRejected: {
                                                                    colorDialog.close();
                                                                }
                                                                Component.onCompleted: visible = true
                                                            }',
                                                           light,
                                                           "colorChooser");
                    }
                    else {
                        if(_color == "white")
                            strips.set(_index,{_color: "yellow"});
                        else
                            strips.set(_index,{_color: "white"});
                    }
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
