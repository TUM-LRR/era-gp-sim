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

#ifndef REGISTERITEM_H
#define REGISTERITEM_H

#include <string>
#include <vector>
#include "ui/registerdata.hpp"

/**
 * Represents an item in the RegisterModel corresponding to a single register.
 */
class RegisterItem {
 public:
  explicit RegisterItem(const RegisterData &data,
                        std::string parentItemIdentifier,
                        std::vector<std::string> childItemIdentifiers);

  std::string getParentItemIdentifier();
  std::string getChildItemIdentifier(int row);
  int childCount() const;
  RegisterData getData() const;
  int getRowOfChild(std::string childIdentifier);

 private:
  /// Register identifier pointing to the register's parent.
  std::string _parentItemIdentifier;
  /// Register identifiers each pointing to the register's children.
  std::vector<std::string> _childItemIdentifiers;
  /// Holding the register's QML-compatable contents.
  RegisterData _itemData;
};

#endif// REGISTERITEM_H
