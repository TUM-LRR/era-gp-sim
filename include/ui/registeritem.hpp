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

  /**
   * @brief getParentItemIdentifier Returns the identifier of the register's
   * parent register.
   * @return The parent register's identifier.
   */
  std::string getParentItemIdentifier();

  /**
   * @brief getChildItemIdentifier Returns the identifier of the register's
   * child register at a given row index.
   * @param row The row index of the requested child register.
   * @return The child register's identifier.
   */
  std::string getChildItemIdentifier(int row);

  /**
   * @brief childCount Returns the number of children of this register.
   */
  int childCount() const;

  /**
   * @brief getData Returns this item's associated QML-compatable data object.
   */
  RegisterData getData() const;

  /**
   * @brief getRowOfChild Returns the row index for a child register specified
   * by the given identifier.
   * @param childIdentifier The child's identifier whose row is requested.
   * @return The row index of the child.
   */
  int getRowOfChild(std::string childIdentifier);

 private:
  /// Register identifier pointing to the register's parent.
  std::string _parentItemIdentifier;
  /// Register identifiers each pointing to one of the register's children.
  std::vector<std::string> _childItemIdentifiers;
  /// Holding the register's QML-compatable contents.
  RegisterData _itemData;
};

#endif// REGISTERITEM_H
