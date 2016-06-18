import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

ToolBar {
    Component{
        id: styleNotClicked
        ButtonStyle{
            background: Rectangle{
            color: "white"
            anchors.fill: parent
            }
        }
    }

    Component{
        id: styleClicked
        ButtonStyle{
            background: Rectangle{
            color: "lightblue"
            anchors.fill: parent
            }
        }
    }

    RowLayout{
        spacing: 1
        anchors.fill: parent
        ToolButton{
            text:"Button1"
            onClicked: {
                console.info("Button 1 clicked");
            }
        }

        Item{ width: 5}

        ToolButton{
            text:"Button2"
            onClicked: {
                console.info("Button2 clicked");
            }
        }

        Item{ width: 5}

        ToolButton{
            text:"Button3"
            onClicked: {
                console.info("Button3 clicked");
            }
        }

        Item{ width: 5}

        ToolButton{
            text:"Button4"
            onClicked: {
                console.info("Button4 clicked");
            }
        }

        Item{ Layout.fillWidth: true}

        ToolButton{
            id: bin
            style: styleClicked
            text: "           "
            Text{
                text: "Bin"
                font.bold: true
                anchors.centerIn: parent
                font.pixelSize: 16
            }

            onClicked: {
                console.info("Bin clicked");
                style=styleClicked;
                oct.notClicked();
                dec.notClicked();
                hex.notClicked();

            }

            function notClicked(){
                style=styleNotClicked;
            }

        }


        ToolButton{
            id: oct
            style: styleNotClicked
            text: "          "
            Text{
                text: "Oct"
                font.bold: true
                anchors.centerIn: parent
                font.pixelSize: 16
            }
            onClicked: {
                console.info("Oct clicked");
                style=styleClicked;
                bin.notClicked();
                dec.notClicked();
                hex.notClicked();
            }

            function notClicked(){
                style=styleNotClicked;
            }


        }

        ToolButton{
            id: dec
            text: "           "
            Text{
                text: "Dec"
                font.bold: true
                anchors.centerIn: parent
                font.pixelSize: 16
            }
            style: styleNotClicked
            onClicked: {
                console.info("Dec clicked");
                style=styleClicked;
                bin.notClicked();
                oct.notClicked();
                hex.notClicked();
            }

            function notClicked(){
                style=styleNotClicked;
            }
        }

        ToolButton{
            id: hex
            text: "           "
            Text{
                text: "Hex"
                font.bold: true
                anchors.centerIn: parent
                font.pixelSize: 16
            }
            style: styleNotClicked
            onClicked: {
                console.info("Hex clicked");
                style=styleClicked;
                bin.notClicked();
                oct.notClicked();
                dec.notClicked();
            }

            function notClicked(){
                style=styleNotClicked;
            }
        }
    }
}
