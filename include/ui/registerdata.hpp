#ifndef REGISTERDATA_H
#define REGISTERDATA_H

#include <QString>

/**
 * @brief Contains the data associated with a single register and held by a
 * RegisterItem inside a RegisterModel.
 */
class RegisterData {
 public:
  RegisterData(QString title, QString content, QString displayFormatString);
  RegisterData();
  QString getTitle() const;
  QString getContent() const;
  QString getDisplayFormatString() const;
  int columnCount() const;

 private:
  QString _title;
  QString _content;
  QString _displayFormatString;
};

#endif// REGISTERDATA_H
