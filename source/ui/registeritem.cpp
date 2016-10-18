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
 * along with this program. If not, see http://www.gnu.org/licenses/.*/

#include "ui/registeritem.hpp"

RegisterItem::RegisterItem(const RegisterData &data, RegisterItem *parent) {
  _parentItem = parent;
  _itemData   = data;
}

RegisterItem::~RegisterItem() {
  qDeleteAll(_childItems);
}

void RegisterItem::appendChild(RegisterItem *item) {
  _childItems.append(item);
}

RegisterItem *RegisterItem::getChild(int row) {
  return _childItems.value(row);
}

int RegisterItem::childCount() const {
  return _childItems.count();
}

int RegisterItem::columnCount() const {
  return _itemData.columnCount();
}

RegisterData RegisterItem::getData() const {
  return _itemData;
}

RegisterItem *RegisterItem::getParentItem() {
  return _parentItem;
}

int RegisterItem::getRow() const {
  if (_parentItem) {
    return _parentItem->_childItems.indexOf(const_cast<RegisterItem *>(this));
  }
  return 0;
}
