#include "ui/registerdata.hpp"

RegisterData::RegisterData(QString title,
                           QString content,
                           QString displayFormatString) {
  _title               = title;
  _content             = content;
  _displayFormatString = displayFormatString;
}

RegisterData::RegisterData() {
  _title               = "";
  _content             = "";
  _displayFormatString = "";
}

QString RegisterData::getTitle() const {
  return _title;
}


QString RegisterData::getContent() const {
  return _content;
}


QString RegisterData::getDisplayFormatString() const {
  return _displayFormatString;
}


int RegisterData::columnCount() const {
  return 3;
}
