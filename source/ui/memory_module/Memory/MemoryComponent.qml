import QtQuick 2.6
import QtQuick.Controls 1.5

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
                                                     16).toUpperCase(), 5)
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
        alternatingRowColors: false
        anchors.fill: parent
        selectionMode: SelectionMode.NoSelection
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

        TableViewColumn {
            role: "address"
            title: "Adresse"
            movable: false
            resizable: false
            width: 80
        }
        TableViewColumn {
            role: "content"
            title: "Inhalt"
            movable: false
            resizable: false
            width: parent.width - 120
            delegate: editableContent
        }
        model: libraryModel
    }

    Component {
        id: editableContent
        TextField {
            inputMask: "\\0\\xHHHH"
            onFocusChanged: cursorPosition=2
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
