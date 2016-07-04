import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import "Components"
import "Menubar"
import "Toolbar"

ApplicationWindow {
    id: window
    visible: true
    menuBar: Menubar{
        component: tabs
        tabView: tabView
    }
    toolBar: ToolbarMainWindow{
        id: toolbar
    }

    TabView{
        anchors.fill: parent
        id: tabView
        property int counter: 1
        function increase(){
            counter=counter+1;
        }

    }


    Component{
        id: tabs
        Splitview{
            anchors.fill: parent
        }
    }
}
