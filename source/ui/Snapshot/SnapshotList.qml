import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Item {//Modul, which should be bound to the container

        width: 250
        height: 300
        anchors.fill: parent


        Rectangle{//Name of the Module
            id: modulName
            height:35
            Text {
                anchors.fill: parent
                text: "Snapshots"
                color: "gray"
                font.bold: true
                font.pixelSize: 12
            }
        }

        Component {//Shows, how the the entries should be presented
            id: listDelegate

            MouseArea {//MouseArea ofer every line for showing buttons
                height: 25
                width: 250
                hoverEnabled: true
                onEntered: {
                    deleteButton.visible=true;
                    loadButton.visible=true;
                    mouseText.width=80;

                }
                onExited: {
                    deleteButton.visible=false;
                    loadButton.visible=false;
                    mouseText.width=250;
                }



                Row {



                    Column{//Space before the text
                        Rectangle{
                            height: 20
                            width: 10
                            color: "white"
                        }
                    }

                    Column{//Text, Snapshot Name

                        MouseArea{
                            id: mouseText
                            height: 20
                            width: 250

                            acceptedButtons: Qt.RightButton | Qt.LeftButton
                            Text{ text: name
                                font.bold: true
                            }

                            onDoubleClicked: {//open and delete by clicks
                                if(mouse.button===Qt.LeftButton){
                                    console.info("Left Double Click, load "+name);
                                }
                                else if(mouse.button===Qt.RightButton){
                                    console.info("Right Double Click, delete "+ name);
                                }
                             }
                        }
                    }
                    Column{//Leaving space to the right end of the window
                        id: afterText
                        Rectangle{
                            height: 20
                            width: 5
                            color: "white"
                        }
                    }
                    Column{
                        Button{
                            id: deleteButton
                            visible: false
                            height: 20
                            width: 40
                            Text{
                                text: "Delete"
                                color: "red"
                                anchors.centerIn: parent
                            }
                            onClicked: {
                                console.info("Button Delete Clicked, delete "+name);
                            }
                        }
                    }
                    Column{//space between the buttons

                        Rectangle{
                            height: 20
                            width: 5
                            color: "white"
                        }
                    }
                    Column{
                        Button{
                            id: loadButton
                            visible: false
                            height: 20
                            width: 40
                            Text{
                                text: "Load"
                                color: "green"
                                anchors.centerIn: parent
                            }
                            onClicked: {
                                console.info("Button Load Clicked, load "+name);
                            }
                        }
                    }

                }

            }
        }



        ListModel {//Data for the View

                 id: listModel

                 ListElement {
                     name: "Apple"
                 }
                 ListElement {
                     name: "Banana"
                 }
                 ListElement {
                     name: "Strawberry"
                 }
                 ListElement {
                     name: "Cherry"
                 }
                 ListElement {
                     name: "Tangerine"
                 }
                 ListElement {
                     name: "Pineapple"
                 }
                 ListElement {
                     name: "Mango"
                 }
                 ListElement {
                     name: "Cranberries"
                 }
                 ListElement {
                     name: "Raspberry"
                 }
                 ListElement {
                     name: "Blackberry"
                 }
                 ListElement {
                     name: "Pear"
                 }
                 ListElement {
                     name: "Peach"
                 }
                 ListElement {
                     name: "Lemon"
                 }
                 ListElement {
                     name: "Lime"
                 }
                 ListElement {
                     name: "Aprciot"
                 }
                 ListElement {
                     name: "Kiwi"
                 }
                 ListElement {
                     name: "Melon"
                 }
                 ListElement {
                     name: "Passion fruit"
                 }
                 ListElement {
                     name: "Pomegranate"
                 }
                 ListElement {
                     name: "Coconut"
                 }
                 ListElement {
                     name: "Plum"
                 }
                 ListElement {
                     name: "Grape"
                 }
                 ListElement {
                     name: "Currant"
                 }
         }


        ListView {
              id: listView
              anchors.top: modulName.bottom//Leavs place for the moduleName
              anchors.left: parent.left
              anchors.bottom: parent.bottom
              anchors.right: parent.right
              model: listModel
              delegate: listDelegate
        }



        function add(name){//Function for adding Data from cpp
            listModel.append({name: name});
        }

        function remove(index){//Function for deleting data from cpp !The index must be known!
            listModel.remove(index);
        }
}
