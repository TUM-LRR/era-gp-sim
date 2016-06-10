import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4

Item {
    property var memory_size: 10
    onMemory_sizeChanged: {
        if (memory_size < 0) {
            memory_size = 0
        }

        while (libraryModel.count < memory_size) {
            libraryModel.append({
                                    address: "0x" + pad(
                                                 libraryModel.count.toString(
                                                     16).toUpperCase(), 5),
                                    info: "info"
                                })
        }
        while (libraryModel.count > memory_size) {
            libraryModel.remove(libraryModel.count - 1, 1)
        }
    }

    function pad(n, width, z) {
        z = z || '0'
        n = n + ''
        return n.length >= width ? n : new Array(width - n.length + 1).join(
                                       z) + n
    }

    ListModel {
        id: libraryModel
    }

    TableView {

        // alternatingRowColors: false
        anchors.fill: parent
        selectionMode: SelectionMode.NoSelection
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

        TableViewColumn {
            role: "address"
            title: "Adresse"
            movable: false
            resizable: false
            width: 70
        }
        TableViewColumn {
            role: "content"
            title: "Inhalt"
            movable: false
            resizable: false
            width: 80
            delegate: editableContent
        }
        TableViewColumn {
            role: "info"
            title: "Info"
            movable: false
            resizable: false
            width: parent.width - 200
        }
        model: libraryModel
    }

    Component {
        id: editableContent
        TextField {

            style: TextFieldStyle {
                background: Rectangle {
                    id: styleRec
                    color: "transparent"
                }
            }
            font.bold: true
            inputMask: "\\0\\xHHHH"
            onActiveFocusChanged: {
                cursorPosition = 2
            }
            onHoveredChanged: {

                //styleRec.border.width=2
            }

            placeholderText: "0x0000"
            text: "0x0000"
        }
    }

    Button {
        text: "+"
        x: 200
        width: 30
        height: 30
        onClicked: memory_size += 10
    }
    Button {
        width: 30
        height: 30
        x: 240
        text: "-"
        onClicked: memory_size -= 10
    }
}
