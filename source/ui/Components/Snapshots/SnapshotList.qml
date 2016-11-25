/*
 * C++ Assembler Interpreter
 * Copyright (C) 2016 Chair of Computer Architecture
 * at Technical University of Munich
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* This file contains the Snapshots */


import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

/*Modul, which should be bound to the container*/
Item {

        width: 250
        height: 300
        anchors.fill: parent

        /*holds the name of the Module*/
        Rectangle{
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

        /* react to signals from guiProject */
        Connections {
          target: guiProject
          onSnapshotsChanged: {
            listView.model = guiProject.getSnapshots();
          }
        }

        /*Shows, how the the entries should be presented*/
        Component {
            id: listDelegate

            /*MouseArea ofer every line for showing buttons*/
            MouseArea {
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
                    /*Space before the text*/
                    Column{
                        Rectangle{
                            height: 20
                            width: 10
                            visible: false
                        }
                    }

                    /*Text, Snapshot Name*/
                    Column{
                        MouseArea{
                            id: mouseText
                            height: 20
                            width: 250

                            acceptedButtons: Qt.RightButton | Qt.LeftButton
                            Text{ text: model.modelData
                                font.bold: true
                            }

                            /*open and delete by clicks*/
                            onDoubleClicked: {
                                if(mouse.button===Qt.LeftButton){
                                    console.info("Left Double Click, load "+model.modelData);
                                    guiProject.loadSnapshot(model.modelData);
                                }
                                else if(mouse.button===Qt.RightButton){
                                    console.info("Right Double Click, delete "+ model.modelData);
                                    guiProject.removeSnapshot(model.modelData);
                                }
                             }
                        }
                    }
                    /*Leaving space to the right end of the window*/
                    Column{
                        id: afterText
                        Rectangle{
                            height: 20
                            width: 5
                            visible: false
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
                                console.info("Button Delete Clicked, delete "+model.modelData);
                                guiProject.removeProject(model.modelData);

                            }
                        }
                    }

                    /*space between the buttons*/
                    Column{
                        Rectangle{
                            height: 20
                            width: 5
                            visible: false
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
                                console.info("Button Load Clicked, load "+model.modelData);
                                guiProject.loadSnapshot(model.modelData);
                            }
                        }
                    }

                }

            }
        }






        ListView {
              id: listView
              /*Leavs place for the moduleName*/
              anchors.top: modulName.bottom
              anchors.left: parent.left
              anchors.bottom: parent.bottom
              anchors.right: parent.right
              model: guiProject.getSnapshots();
              delegate: listDelegate
        }


        /*Function for adding Data from cpp*/
        function add(name){
            listModel.append({name: name});
        }

        /*Function for deleting data from cpp !The index must be known!*/
        function remove(index){
            listModel.remove(index);
        }
}
