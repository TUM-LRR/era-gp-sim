import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

SplitView{
    orientation: Qt.Horizontal

//    SplitView{
//        Layout.minimumWidth: 10
//        width: parent.width/4
//        id: splitView1
//        orientation: Qt.Vertical
//        //Sp{
//            //anchors.fill: parent
//           // usual: "0"
//        //}
//        SplitviewItem{
//            height: parent.height/2
//            Layout.minimumHeight: 25
//            usual: "0"
//        }
//        Rectangle{
//            Layout.minimumHeight: 25
//            color: "blue"
//            height: parent.height/2
//        }

//    }

    InnerSplitviews{
        Layout.minimumWidth: 10
        width: parent.width/4
        id: splitView1
        faktor: 2
        quotient: 3
        usual1: "snapshots"
        usual2: "output"

    }


    InnerSplitviews{
        Layout.minimumWidth: 10
        width: parent.width/4
        id: splitView2
        faktor: 2
        quotient: 3
        usual1: "editor"
        usual2: "output"

    }

    InnerSplitviews{
        Layout.minimumWidth: 10
        width: parent.width/4
        id: splitView3
        faktor: 1
        quotient: 2
        usual1: "register"
        usual2: "register"
    }


    InnerSplitviews{
        Layout.minimumWidth: 10
        width: parent.width/4
        id: splitView4
        faktor: 1
        quotient: 2
        usual1: "memory"
        usual2: "memory"
    }

    onWidthChanged: {
        splitView1.width=(width/10);
        splitView2.width=5*(width/10);
        splitView3.width=2*(width/10);
        splitView4.width=2*(width/10);
    }



//    SplitView{
//        id: splitView3
//        orientation: Qt.Vertical
//    }

//    SplitView{
//        id: splitview4
//        orientation: Qt.Vertical
//    }


}
