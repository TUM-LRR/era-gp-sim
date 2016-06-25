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
        usual1: "leer"
        usual2: "snapshots"
        onWidthChanged: {
            console.info("1 "+parent.width+"   "+width);
        }
    }


    InnerSplitviews{
        Layout.minimumWidth: 10
        width: parent.width/4
        id: splitView2
        usual1: "leer"
        usual2: "output"
        onWidthChanged: {
            console.info("2 "+parent.width+"   "+width);
        }
    }

    InnerSplitviews{
        Layout.minimumWidth: 10
        width: parent.width/4
        id: splitView3
        usual1: "leer"
        usual2: "snapshots"
        onWidthChanged: {
            console.info("1 "+parent.width+"   "+width);
        }
    }


    InnerSplitviews{
        Layout.minimumWidth: 10
        width: parent.width/4
        id: splitView4
        usual1: "leer"
        usual2: "output"
    }

    onWidthChanged: {
        splitView1.width=width/4;
        splitView2.width=width/4;
        splitView3.width=width/4;
        splitView4.width=width/4;
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
