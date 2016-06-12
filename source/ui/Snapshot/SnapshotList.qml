import QtQuick 2.0
import QtQuick.Dialogs 1.2

Item {
    //Rectangle {
        width: 250
        height: 300
        anchors.fill: parent
        //color: "blue"


        Component {

            id: listDelegate

            Item {
                //anchors.left: parent
                height: 20

                Row {

                    MouseArea{
                        height: 20
                        width: listView.width
                        acceptedButtons: Qt.RightButton | Qt.LeftButton
                        Text{ text: 'Name: ' +name}

                        onDoubleClicked: {
                            if(mouse.button===Qt.LeftButton){
                                console.info("Left Double Click, load "+name);
                                //listModel.append({name: "Test"});
                                //listModel.remove(1);
                            }
                            else if(mouse.button===Qt.RightButton){
                                console.info("Right Double Click, delete "+ name);
                            }
                        }

//                        onClicked: {
//                            if(mouse.button===Qt.LeftButton){
//                                console.info("Left Click, do nothing "+name);
//                            }
//                            else if(mouse.button === Qt.RightButton){
//                                console.info("Right click, delete "+ name);
//                            }
//                        }
                    }

                    //Text{ text: 'Name: '+ name}

                }
            }
        }

        ListModel {

                 id: listModel

                 ListElement {
                     name: "Apple"
                 }
                 ListElement {
                     name: "Banana"
                 }
         }

        ListView {
              id: listView
              anchors.fill: parent
              //anchors.margins: 5
              model: listModel
              delegate: listDelegate


              //focus: true
        }

        function add(name){
            listModel.append({name: name});
        }

        function remove(index){
            listModel.remove(index);
        }

    //}



}
