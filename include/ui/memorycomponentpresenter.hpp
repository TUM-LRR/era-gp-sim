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
//#include "MemoryValue"

class MemoryComponentPresenter : public QAbstractTableModel{
    Q_OBJECT

    public:
       explicit MemoryComponentPresenter(/* Coreschnittstelle *core */ QQmlContext *context, QObject *parent = 0);
       ~MemoryComponentPresenter();
       void setSize(int newSize);
       void setValue(int address, /*MemoryValue*/ int newValue);
       void setContextInformation(int addressStart, int length, int identifier);

       int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
       int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
       QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

#endif /* MEMORYCOMPONENTPRESENTER_HPP_ */
