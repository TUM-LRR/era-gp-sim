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

    onValueChanged: {
        textField.text=value.toString();
    }
    onMinValueChanged: {
        //fit value into value boundary given by minValue und maxValue
        if(value < minValue)
            value = minValue
    }
    onMaxValueChanged: {
        //fit value into value boundary given by minValue und maxValue
        if(value > maxValue)
            value = maxValue
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
    }
    Button {
        anchors.right: parent.right
        anchors.top: parent.top
        height: parent.height / 2
        width: 20
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
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: parent.height / 2
        width: 20
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
