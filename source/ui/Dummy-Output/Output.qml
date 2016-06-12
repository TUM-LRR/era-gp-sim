import QtQuick 2.0
import QtQuick.Controls 1.4

Item {

    GreenRectangle{
        id: greenBackground
        anchors.fill: parent
        visible: false
        enabled: false
    }

    BlueRectangle{
        id: blueBackground
        anchors.fill: parent
        visible: false
        enabled: false
    }

    RedRectangle{
        id: redBackground
        anchors.fill: parent
        visible: false
        enabled: false

    }
    ComboBox{
        width: 150
        model: ["","Red Rectangle", "Blue Rectangle", "GreenRectangle" ]

        onCurrentIndexChanged:{
            if(currentIndex==0){
                redBackground.visible=false;
                redBackground.enabled=false;
                blueBackground.visible=false;
                blueBackground.enabled=false;
                greenBackground.visible=false;
                greenBackground.enabled=false;
            }else if(currentIndex==1){
                redBackground.visible=true;
                redBackground.enabled=true;
                blueBackground.visible=false;
                blueBackground.enabled=false;
                greenBackground.visible=false;
                greenBackground.enabled=false;
            }else if(currentIndex==2){
                redBackground.visible=false;
                redBackground.enabled=false;
                blueBackground.visible=true;
                blueBackground.enabled=true;
                greenBackground.visible=false;
                greenBackground.enabled=false;
            }else if(currentIndex==3){
                redBackground.visible=false;
                redBackground.enabled=false;
                blueBackground.visible=false;
                blueBackground.enabled=false;
                greenBackground.visible=true;
                greenBackground.enabled=true;
            }
        }
    }


}
