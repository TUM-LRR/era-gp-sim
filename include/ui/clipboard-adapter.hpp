#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QClipboard>
#include <QApplication>

class ClipboardAdapter: public QObject
{
    Q_OBJECT
public:
    explicit ClipboardAdapter(QObject *parent = 0);

    Q_INVOKABLE void setText(QString text);

    Q_INVOKABLE QString text(QClipboard::Mode mode = QClipboard::Clipboard);

public slots:
    void setData();

signals:
    Q_INVOKABLE void dataChanged();

private:
    QClipboard *clipboard;
};

#endif // CLIPBOARD_H
