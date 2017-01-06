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
        anchors.fill: parent

        /*holds the name of the Module*/
        Rectangle{
            id: moduleName
            anchors.top: parent.top
            height: 20
            Label {
                text: "Snapshots"
                color: "gray"
                font.bold: true
                font.pixelSize: 12
            }
        }

        /*Add a snapshot */
        Button {
          id: addButton
          anchors.top: moduleName.bottom
          anchors.horizontalCenter: parent.horizontalCenter
          height: 30
          text: "Create Snapshot"
          onClicked: {
            window.menubar.actionSnapshot();
          }
        }

        /* react to signals from guiProject */
        Connections {
            target: snapshotComponent
            onSnapshotsChanged: {
              listView.model = guiProject.getSnapshots();
            }
        }

        /* Defines how the the entries should be presented*/
        Component {
            id: listDelegate
            Item {
                property alias text: textItem.text
                height: textItem.height
                width: listView.width
                /* The name of the snapshot */
                Label {
                    id: textItem
                    anchors.left: parent.left
                    anchors.leftMargin: 3
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.modelData
                    font.bold: true
                }

                /* Mouse are to check if the mouse hovers above this list entry */
                MouseArea {
                  id: mouseArea
                  anchors.fill: parent
                  // has to be above the highlight component.
                  z: listView.z + 1
                  hoverEnabled: true
                  acceptedButtons: Qt.NoButton
                  propagateComposedEvents: true
                  onEntered: {
                     listView.currentIndex = index;
                     loadButton.visible = true;
                     deleteButton.visible = true;
                  }
                  onExited: {
                    if(listView.currentIndex==index) {
                      listView.currentIndex = -1;
                    }
                    loadButton.visible = false;
                    deleteButton.visible = false;
                  }

                  /*Buttons to load or delete a snapshot
                    These buttons can't be in the highlight component, as the
                    text of this snapshot entry would be above them.
                   */
                  Button {
                      id: loadButton
                      anchors.right: deleteButton.left
                      anchors.verticalCenter: parent.verticalCenter
                      height: textLoad.height + 2
                      width: textLoad.width + 5
                      visible: false
                      Label {
                          id: textLoad
                          text: "Load"
                          color: "green"
                          anchors.centerIn: parent
                      }
                      onClicked: {
                          guiProject.loadSnapshot(model.modelData);
                      }
                  }
                  Button {
                      id: deleteButton
                      anchors.right: parent.right
                      anchors.verticalCenter: parent.verticalCenter
                      height: textDelete.height + 2
                      width: textDelete.width + 5
                      visible: false
                      Label {
                          id: textDelete
                          text: "Delete"
                          color: "red"
                          anchors.centerIn: parent
                      }
                      onClicked: {
                          guiProject.removeSnapshot(model.modelData);
                      }
                    }
                }
            }
        }

        /* A component to display a highlight on mouse hover. */
        Component {
          id: highlightComponent
          Rectangle {
            color: Qt.rgba(0.7, 0.7, 0.7, 0.2)
            radius: 5
            MouseArea{
                id: mouseText
                anchors.fill: parent
                acceptedButtons: Qt.AllButtons
                onDoubleClicked: {
                    if(mouse.button===Qt.LeftButton){
                      if(listView.currentItem != null) {
                        guiProject.loadSnapshot(listView.currentItem.text);
                      }
                    }
                    else if(mouse.button===Qt.RightButton){
                      if(listView.currentItem != null) {
                        guiProject.removeSnapshot(listView.currentItem.text);
                      }
                    }
                 }
            }
          }
        }

        /* The list view to display a list of snapshots. */
        ListView {
              id: listView
              /*Leaves place for the moduleName*/
              anchors.topMargin: 10
              anchors.top: addButton.bottom
              anchors.left: parent.left
              anchors.bottom: parent.bottom
              anchors.right: parent.right
              model: guiProject.getSnapshots();
              highlight: highlightComponent
              highlightFollowsCurrentItem: true
              currentIndex: -1
              delegate: listDelegate
        }
}
