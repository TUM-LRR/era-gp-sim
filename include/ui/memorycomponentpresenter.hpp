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

#include <QAbstractListModel>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QQmlContext>
#include <QVariant>
#include "core/memory-access.hpp"
#include "core/memory-manager.hpp"

class MemoryComponentPresenter : public QAbstractListModel {
  Q_OBJECT

 public:
  explicit MemoryComponentPresenter(MemoryAccess access,
                                    MemoryManager manager,
                                    QQmlContext *projectContext,
                                    QObject *parent = 0);
  ~MemoryComponentPresenter();
  void setSize(int newSize);
  Q_INVOKABLE void setValue(int address, QString number);
  void setContextInformation(int addressStart, int length, int identifier);

 private:
  // TODO: memory object from core
  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
  QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
  void onMemoryChanged(const std::size_t address, const std::size_t length);

  MemoryAccess _memoryAccess;
  MemoryManager _memoryManager;

  /** saves the size of the memory, as calling MemoryAccess::getMemorySize() in
   * rowCount causes a deadlock. */
  std::size_t _memorySize;

  enum ColumnRoles {
    AddressRole = Qt::UserRole,// avoid collisions with predefined roles
    ValueRole,
    InfoRole
  };
};

#endif /* MEMORYCOMPONENTPRESENTER_HPP_ */
