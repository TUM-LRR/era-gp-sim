import QtQuick 2.0
import QtQuick.Controls 1.4

Item {//The Output-Module

    //Dummys, created by starting the window because of time reasons. They will not be active unless they are choosen
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


    ComboBox{//Drop-Down button
        width: 150
        model: ["Choose Output","Red Rectangle", "Blue Rectangle", "GreenRectangle" ]

        onCurrentIndexChanged:{//If another line is choosen, an other Output will be set active, al others stay inactive
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
