import QtQuick 2.0
import QtQuick.Controls 1.4
import "Components"

Item {

    property var usual



    Rectangle{
        id: space
        height: 10
        width: 150
        visible: false
    }



    ComboBox{
        id: box
        visible: true
        enabled: true
        anchors.top: space.bottom
        width: 150
        height: 20
        model: ["Choose Component","Snapshots", "Output", "Editor", "Register", "Memory" ]

        onCurrentIndexChanged:{
            if(currentIndex==0){
                holder.change("nothing");
            }else if(currentIndex==1){
                holder.change("snapshots");
            }else if(currentIndex==2){
                holder.change("output");
            }
            else if(currentIndex==3){
                holder.change("editor");
            }
            else if(currentIndex==4){
                holder.change("register");
            }
            else{
                holder.change("memory");
            }

        }
    }

    Rectangle{
        id: rect
        anchors.top: box.bottom
        width: 150
        height: 10
        visible: false
    }

    ComponentHolder{
        id: holder
        anchors.top: rect.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        usual: parent.usual
    }





}

