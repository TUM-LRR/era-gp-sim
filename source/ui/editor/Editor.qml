import QtQuick 2.6
import QtQuick.Controls 1.5

TextArea {
    id: textArea
    textFormat: TextEdit.PlainText
    wrapMode: TextEdit.WrapAnywhere
    Component.onCompleted: project.createHighlighter(textDocument)
}
