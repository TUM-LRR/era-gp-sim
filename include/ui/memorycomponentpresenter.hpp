/*
* C++ Assembler Interpreter
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MEMORYCOMPONENTPRESENTER_HPP_
#define MEMORYCOMPONENTPRESENTER_HPP_

#include <QQmlContext>
#include <QVariant>
#include <QObject>
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QHash>
#include "core/memory.hpp"

class MemoryComponentPresenter : public QAbstractTableModel{
    Q_OBJECT

    public:
       explicit MemoryComponentPresenter(Memory *memory, QQmlContext *context, QObject *parent = 0);
       ~MemoryComponentPresenter();
       void setSize(int newSize);
       void setValue(int address, /*TODO MemoryValue*/ int newValue);
       void setContextInformation(int addressStart, int length, int identifier);

    private:
       // TODO: memory object from core
       int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
       int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
       QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
       QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
       void onMemoryChanged(const std::size_t address, const std::size_t length);

       Memory *corememory;

       enum ColumnRoles {
           AddressRole=Qt::UserRole,//avoid collisions with predefined roles
           ValueRole,
           InfoRole
       };
};

#endif /* MEMORYCOMPONENTPRESENTER_HPP_ */
