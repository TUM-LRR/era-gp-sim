/* C++ Assembler Interpreter
 * Copyright (C) 2016 Chair of Computer Architecture
 * at Technical University of Munich
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 */

#ifndef REGISTERDATA_H
#define REGISTERDATA_H

#include <QString>
#include <QStringList>

/**
 * @brief Contains the data associated with a single register and held by a
 * RegisterItem inside a RegisterModel.
 */
class RegisterData {
 public:
  RegisterData(QString title,
               QString content,
               QString displayFormatString,
               QStringList dataFormatsList = QStringList() << "Hex"
                                                           << "Dec"
                                                           << "Bin");
  RegisterData();
  QString getTitle() const;
  QString getContent() const;
  QString getDisplayFormatString() const;
  QStringList getDataFormatsList() const;

 private:
  /// The register's title (e.g. "EAX")
  QString _title;
  /// The register's content (e.g. "AB 01 CD 20" or "10011001")
  QString _content;
  /// A formatting rule for aligning the register's content (e.g. "HH HH HH HH"
  /// for Hex-alignment)
  QString _displayFormatString;
  /// A list of available data formats (e.g. "Dec, Hex, Bin")
  QStringList _dataFormatsList;
};

#endif// REGISTERDATA_H
