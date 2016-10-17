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

#include "ui/memorycomponentpresenter.hpp"
#include <iostream>
#include <QDebug>
#include "core/memory-value.hpp"

MemoryComponentPresenter::MemoryComponentPresenter(const MemoryAccess access, /*const MemoryManager manager,*/ QQmlContext *context, QObject *parent)
: QAbstractTableModel(parent){
    // TODO register callback functions
    //memory->setCallback(&MemoryComponentPresenter::onMemoryChanged);

   memory_access = access;
   //memory_manager = manager;

    // TODO create new View
    //context->setContextProperty("MemoryComponent", this);
    this->setValue(5,10);
}


MemoryComponentPresenter::~MemoryComponentPresenter() { }

void onMemoryChanged(const std::size_t address, const std::size_t length) {
    // emit signal, damit die data() methode im qt-thread läuft
}

void MemoryComponentPresenter::setSize(int newSize) {
    // TODO
}

void MemoryComponentPresenter::setValue(int address, /*TODO MemoryValue*/ int newValue) {
    this->dataChanged(this->index(address, 0), this->index(address, 2));
}

void MemoryComponentPresenter::setContextInformation(int addressStart, int length, int identifier) {
    this->dataChanged(this->index(addressStart, 0), this->index(addressStart + length, 2));
    // TODO
}



int MemoryComponentPresenter::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return corememory->getByteCount();
}
int MemoryComponentPresenter::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    // 3: address + value + additional information
    return 3;
}
QVariant MemoryComponentPresenter::data(const QModelIndex &index, int role) const {
    // check boundaries
    if (!index.isValid()) {
        qWarning() << "Warning: " << index.row() << ", " << index.column();
        return QVariant();
    }

    switch(role)
    {
        case AddressRole: {
            // format index as hex value and return it
            return QString("%1").arg(index.row(), 4, 16, QLatin1Char('0')).toUpper().prepend("0x");
        }
        case ValueRole: {
            // TODO fetch value from core
            MemoryValue v = corememory->get(index.row());

            return  QString;
        }
        case InfoRole: {
            // TODO fetch value from core
            return  QString("information");
        }
        default: {
            qWarning() << "unknown column role";
            return QVariant();
        }
    }
}
QHash<int, QByteArray> MemoryComponentPresenter::roleNames() const {
    // connect TableColumns in View with columns in this model
    QHash<int, QByteArray> roles;
    roles[AddressRole] = "address";
    roles[ValueRole] = "value";
    roles[InfoRole] = "info";
    return roles;
}
