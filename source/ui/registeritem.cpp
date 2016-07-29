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

#include "ui/registeritem.hpp"

RegisterItem::RegisterItem(const RegisterData &data,
                           std::string parentItemIdentifier,
                           std::vector<std::string> childItemIdentifiers) {
  _itemData             = data;
  _parentItemIdentifier = parentItemIdentifier;
  _childItemIdentifiers = childItemIdentifiers;
}

std::string RegisterItem::getParentItemIdentifier() {
  return _parentItemIdentifier;
}

std::string RegisterItem::getChildItemIdentifier(int row) {
  return _childItemIdentifiers[row];
}

int RegisterItem::childCount() const {
  return _childItemIdentifiers.size();
}

RegisterData RegisterItem::getData() const {
  return _itemData;
}

int RegisterItem::getRowOfChild(std::string childIdentifier) {
  auto it = std::find(_childItemIdentifiers.begin(),
                      _childItemIdentifiers.end(),
                      childIdentifier);
  if (it != _childItemIdentifiers.end()) {
    return std::distance(_childItemIdentifiers.begin(), it);
  }
  return -1;
}
