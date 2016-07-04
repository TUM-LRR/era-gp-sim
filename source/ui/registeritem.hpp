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

#ifndef REGISTERITEM_H
#define REGISTERITEM_H

#include <QList>
#include <QVariant>

/**
 * Represents the data corresponding to a single register. Belongs to the RegisterModel.
 */
class RegisterItem
{
public:
    explicit RegisterItem(const QList<QVariant> &data, RegisterItem *parentItem = 0);
    ~RegisterItem();

    void appendChild(RegisterItem *child);

    RegisterItem *getChild(int row);
    int childCount() const;
    int columnCount() const;
    QVariant getData(int column) const;
    int getRow() const;
    RegisterItem *getParentItem();

private:
    QList<RegisterItem*> m_childItems;
    QList<QVariant> m_itemData;
    RegisterItem *m_parentItem;
};

#endif // REGISTERITEM_H
