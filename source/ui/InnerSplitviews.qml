import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2



    SplitView{
        property var usual1
        property var usual2
        property int quotient
        property int faktor


        orientation: Qt.Vertical
        SplitviewItem{
            id: item1
            Layout.minimumHeight: 25
            height: parent.height/2
            usual: parent.usual1
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
            item1.usual=usual1;
        }

        onUsual2Changed: {
            item2.usual=usual2;
        }

        onHeightChanged: {
            item1.height=faktor*(height/quotient)-5;
            item2.height=height-item1.height-5;
        }

    }


