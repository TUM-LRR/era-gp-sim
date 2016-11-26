import QtQuick 2.0
import QtQuick 2.3
import QtQuick.Controls 1.4

Item {
    property int mode: 1


    Rectangle{
        anchors.fill: parent
        color: "black"

        TextArea{
            id: textarea
            height: parent.height
            width: parent.width
            anchors.top: parent.top
            anchors.left: parent.left

            wrapMode: Text.Wrap
            textColor: "white"

            readOnly: true
            backgroundVisible: false
            MouseArea{
                id: clearArea
               anchors.fill:parent
               visible: true
               acceptedButtons: Qt.RightButton
               onDoubleClicked: {
                   consoleModel.clear();
               }
            }

        }

        Connections{
            target: consoleModel
            onTextChanged:{
                textarea.text=consoleModel.getText();
                console.info(textarea.text);
                //console.info("Juhu!");
            }
        }
        Connections{
            target: consoleModel
            onDataChanged: {
                consoleModel.getData();
            }
        }
    }
    onModeChanged: {
        consoleModel.modeChanged(mode);
        if(mode ==0 /*mode == array-based*/){
            clearArea.enabled=false;
        }
        else{
            clearArea.enabled=true;
        }
    }

    ConsoleSettingsWindow{
        id: settingsWindowC
    }

    function settingsButtonPressed(){
        settingsWindowIT.show();
    }

}


