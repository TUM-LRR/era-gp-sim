import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

ApplicationWindow {
    id: window
    visible: true
    menuBar: Menubar{
        component: tabs
        tabView: tabView
    }

    TabView{
        anchors.fill: parent
        id: tabView
    }


    Component{
        id: tabs
        Splitview{
            anchors.fill: parent
        }
    }
}
