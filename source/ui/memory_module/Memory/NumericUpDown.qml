import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1

Item {
    height: 30
    width: 100

    property int _value: 0
    property int _minValue: 0
    property int _maxValue: 10000
    property int _step: 12

    signal valueChanged(int value)
    signal valueBoundariesChanged(int minValue, int maxValue)

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
            _value = parseInt(text)
            console.log("memory size input by user: " + parseInt(text))
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
            if (_value + _step > _maxValue)
                _value = _maxValue
            else
                _value += _step
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
            if (_value - _step < _minValue)
                _value = _minValue
            else
                _value -= _step
        }
    }
}
