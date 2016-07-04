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

#include "registeritem.hpp"

RegisterItem::RegisterItem(const QList<QVariant> &data, RegisterItem *parent) {
    m_parentItem = parent;
    m_itemData = data;
}

RegisterItem::~RegisterItem() {
    qDeleteAll(m_childItems);
}

void RegisterItem::appendChild(RegisterItem *item) {
    m_childItems.append(item);
}

RegisterItem *RegisterItem::getChild(int row) {
    return m_childItems.value(row);
}

int RegisterItem::childCount() const {
    return m_childItems.count();
}

int RegisterItem::columnCount() const {
    return m_itemData.count();
}

QVariant RegisterItem::getData(int column) const {
    return m_itemData.value(column);
}

RegisterItem *RegisterItem::getParentItem() {
    return m_parentItem;
}

int RegisterItem::getRow() const {
    if (m_parentItem) {
        return m_parentItem->m_childItems.indexOf(const_cast<RegisterItem*>(this));
    }
    return 0;
}
