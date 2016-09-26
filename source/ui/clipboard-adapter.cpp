#include "ui/clipboard-adapter.hpp"

ClipboardAdapter::ClipboardAdapter(QObject *parent) : QObject(parent) {
    clipboard = QApplication::clipboard();
    QObject::connect(clipboard, SIGNAL(dataChanged()), this, SLOT(setData()));
}

void ClipboardAdapter::setText(QString text){
    clipboard->setText(text, QClipboard::Clipboard);
    //clipboard->setText(text, QClipboard::Selection);
}


void ClipboardAdapter::setData() {
    emit dataChanged();
}


QString ClipboardAdapter::text(QClipboard::Mode mode) {
    return clipboard->text(mode);
}
