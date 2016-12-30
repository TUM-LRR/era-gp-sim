#include "ui/clipboard-adapter.hpp"

ClipboardAdapter::ClipboardAdapter(QObject *parent) : QObject(parent) {
  clipboard = QApplication::clipboard();
  QObject::connect(clipboard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(setData(QClipboard::Mode)));
}

void ClipboardAdapter::setText(const QString& text) {
  clipboard->setText(text, QClipboard::Clipboard);
}


void ClipboardAdapter::setData(QClipboard::Mode mode) {
  if (mode == QClipboard::Mode::Clipboard) {
    emit dataChanged();
  }
}


QString ClipboardAdapter::text(QClipboard::Mode mode) {
  return clipboard->text(mode);
}
