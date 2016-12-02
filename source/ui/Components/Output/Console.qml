import QtQuick 2.0
import QtQuick 2.3
import QtQuick.Controls 1.4

Item {
    property int outputItemIndex: 2
    property int oldMode: 0
    property var oldAddress: 0
    id: it


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
            MouseArea{//Clears the Screen in pipelike mode
                id: clearArea
               anchors.fill:parent
               visible: true
               acceptedButtons: Qt.RightButton
               onDoubleClicked: {
                   if(outputComponent.getOutputItem(outputItemIndex)["textMode"]!==0){
                       textarea.text = "";
                   }
               }
            }
            function clear(){
                text = "";
            }

        }

        Connections {
            target: outputComponent
            // Send when the memory changes (at any address).
            onMemoryChanged: {
                //console.log("onMemoryChanged");
                var _baseAddress = outputComponent.getOutputItem(outputItemIndex)["baseAddress"];
                var _mode = outputComponent.getOutputItem(outputItemIndex)["textMode"];
                // Check if the memory address that was changed (at least partly) belongs to
                // the output item's source space.
                if ((address+length) >= _baseAddress && _mode == 0) {
                    it.updateContent(_baseAddress);
                }
                else if(_mode != 0 && (address == _baseAddress|| (address<_baseAddress && address+length >= _baseAddress)) ){
                    it.updateContent(_baseAddress);
                }
            }
            // Send when any item's settings where updated.
            onOutputItemSettingsChanged: {
                //console.log("onOutputItemSettingsChanged");
                var mode = outputComponent.getOutputItem(outputItemIndex)["textMode"];
                var baseAddress = outputComponent.getOutputItem(outputItemIndex)["baseAddress"];
                if(mode == oldMode && baseAddress == oldAddress){
                    return;
                }
                it.updateContent(outputComponent.getOutputItem(outputItemIndex)["baseAddress"]);
                settingsWindowC.updateSettings();
            }
        }
    }
    function updateContent(_baseAddress) {
        var mode = outputComponent.getOutputItem(outputItemIndex)["textMode"];
        if(oldMode !== mode){
            oldMode = mode;
            textarea.clear();
        }

        if(oldAddress !== _baseAddress){
            oldAddress = _baseAddress;
            textarea.clear();
        }

        var currentText = textarea.text;
        textarea.text = outputComponent.getTextFromMemory(_baseAddress, currentText, mode);
    }


    ConsoleSettingsWindow{
        id: settingsWindowC
    }

    function settingsButtonPressed(){
        settingsWindowC.show();
    }

}


