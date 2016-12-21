#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QApplication>
#include <QClipboard>

class ClipboardAdapter : public QObject {
  Q_OBJECT
 public:
  explicit ClipboardAdapter(QObject *parent = 0);

  /**
  \brief setText Allows QML instances to set the clipboard
  data.
  \param text New clipboard text.
  */
  Q_INVOKABLE void setText(const QString& text);

  /**
  \brief text Returns the content of the clipboard.
  \param mode Clipboard mode to use.
  */
  Q_INVOKABLE QString text(QClipboard::Mode mode = QClipboard::Clipboard);

 public slots:
  /**
   * \brief setData Slot for reacting to a dataChanged-signal
   * of the QClipboard-instance.
   */
  void setData();

 signals:
  /**
  \brief: Signal that is emitted when the clipboard adapter data
  notices changes to the clipboard (i.e. when user copies text).
  */
  Q_INVOKABLE void dataChanged();

 private:
  /**
   * \brief clipboard The actual clipboard object.
   */
  QClipboard *clipboard;
};

#endif// CLIPBOARD_H
