import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2



    SplitView{
        property var usual1
        property var usual2


        orientation: Qt.Vertical
        Rectangle{
            id: item1
            Layout.minimumHeight: 25
            color: "red"
            height: parent.height/2
        }

        SplitviewItem{
            id: item2
            height: parent.height/2
            Layout.minimumHeight: 10
            usual: parent.usual2
            onUsualChanged: {
                //console.info("SplitviewItem: "+usual+parent.usual2);

            }
        }


        onUsual1Changed: {
            //console.info(usual1);
        }

        onUsual2Changed: {
            item2.usual=usual2;
        }

        onHeightChanged: {
            item1.height=(height/2)-5;
            item2.height=(height/2)-5;
        }

    }


