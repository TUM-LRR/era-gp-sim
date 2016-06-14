import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

Item {
    height: 30
    width: 100

    property int value: 0
    property int minValue: 0
    property int maxValue: 10000
    property int step: 12

    signal valueChanged(int value)
    signal valueBoundariesChanged(int minValue, int maxValue)

    onValueChanged: {
        textField.text=value.toString();
        valueChanged(value)
    }
    onMinValueChanged: {
        //fit value into value boundary given by minValue und maxValue
        if(value < minValue)
            value = minValue
        valueBoundariesChanged(minValue, maxValue)
    }
    onMaxValueChanged: {
        //fit value into value boundary given by minValue und maxValue
        if(value > maxValue)
            value = maxValue
        valueBoundariesChanged(minValue, maxValue)
    }


    TextField{
        id: textField
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 20

        text:"0"

        onAccepted: {
            value = parseInt(text)
            console.log("memory size input by user: "+parseInt(text))
        }

        //enable changing numbers with arrow keys
        Keys.onUpPressed: button_up.clicked()
        Keys.onDownPressed: button_down.clicked()
    }
    Button {
        id: button_up
        anchors.right: parent.right
        anchors.top: parent.top
        height: parent.height / 2 + 1
        width: 20 + 3
        text: "△"

        onClicked: {
            //fit value into value boundary given by minValue und maxValue
            if(value + step > maxValue)
                value = maxValue
            else
                value += step
        }
    }
    Button {
        id: button_down
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: parent.height / 2 + 1
        width: 20 + 3
        text: "▽"

        onClicked: {
            //fit value into value boundary given by minValue und maxValue
            if(value - step < minValue)
                value = minValue
            else
                value -= step
        }
    }

}
