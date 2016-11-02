import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    toolBar: ToolBar {
        anchors.fill: parent

        ToolButton {
            Layout.alignment: Qt.AlignRight
            text: "Add module"
        }
    }

    TabView {
        anchors.fill: parent
        anchors.centerIn: parent

        Tab {
            title: "Example.asm"
            anchors.fill: parent
            SplitView {
                id: moduleSplitView
                anchors.fill: parent
                orientation: Qt.Horizontal
                property var numberOfViews: 2

                Rectangle {
                    width: moduleSplitView.width / numberOfViews
                    Layout.minimumWidth: 100
                    Layout.maximumWidth: moduleSplitView.width - 50
                    color: "lightblue"
                    Text {
                        text: "View 1"
                        anchors.centerIn: parent
                    }
                }

                Component {
                    id: templateModule
                    Rectangle {
                        color: Qt.rgba(Math.random(), Math.random(), Math.random(), 1)
                        width: moduleSplitView.width / numberOfViews
                        Layout.minimumWidth: 100
                        Layout.maximumWidth: moduleSplitView.width - 50
                        ComboBox {
                            id: moduleSwitcher
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.topMargin: 5

                        }

                        Text {
                            text: "Template"
                            anchors.centerIn: parent
                        }
                    }
                }

                Rectangle {
                    id: centerItem
                    width: moduleSplitView.width / moduleSplitView.numberOfViews
                    Layout.minimumWidth: 100
                    Layout.maximumWidth: moduleSplitView.width - 50
                    color: "lightgray"
                    Text {
                        text: "View 2"
                        anchors.centerIn: parent
                    }

                    Button {
                        anchors.centerIn: parent
                        text: "Add Split"
                        onClicked: {
                            var newItem = templateModule.createObject();
                            moduleSplitView.addItem(newItem);
                            moduleSplitView.numberOfViews++;
                            console.log(MyScript.counter);
                        }
                    }
                }
            }
        }

        Tab {
            title: "Example2.asm"
            Rectangle {
                anchors.fill: parent
                anchors.centerIn: parent
                color: "red"
            }
        }
    }


}
