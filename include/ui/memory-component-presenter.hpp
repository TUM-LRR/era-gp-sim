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

#ifndef ERAGPSIM_UI_MEMORY_COMPONENT_PRESENTER_HPP
#define ERAGPSIM_UI_MEMORY_COMPONENT_PRESENTER_HPP

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
  MemoryComponentPresenter(const MemoryAccess &access,
                           const MemoryManager &manager,
                           QQmlContext *projectContext,
                           QObject *parent = 0);

  /**
   * Stores a value to memory.
   *
   * \param address the address of the cell to be updated
   * \param value the new value for the memory cell
   * \param numberOfBits The number of bits used.
   * \param role The current role (format + bits).
   */
  Q_INVOKABLE void setValue(int address,
                            const QString &value,
                            int numberOfBits,
                            const QString &role);

  /**
   * Sets the context information for memory cells (NOT IMPLEMENTED YET)
   *
   * \param addressStart the starting address of the memory cell the context
   * information is related to
   * \param length the number of memory cells the context information is related
   * to
   * \param identifier the unique identifier for this context information (for
   * further updates)
   *
   */
  void setContextInformation(int addressStart, int length, int identifier);

 private:
  /**
   * Returns the data stored under the given role for the item referred to by
   * the index.
   * Inherited from QAbstractListModel
   *
   * \param index the index where the data should be written to
   * \param role one of several DisplayRoles for this column
   * \return returns the QVariant that is displayed in the view
   *
   */
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

  /**
   * helper method: returns address data for data method
   *
   * \param index the index where the data should be written to
   * \param role one of several DisplayRoles for this column
   * \return returns the QVariant that is displayed as address
   */
  QVariant
  dataAdress(const QModelIndex &index, int role = Qt::DisplayRole) const;

  /**
   * helper method: returns memory data for data method
   *
   * \param index the index where the data should be written to
   * \param role one of several DisplayRoles for this column
   * \return returns the QVariant that is displayed as memory
   */
  QVariant
  dataMemory(const QModelIndex &index, int role = Qt::DisplayRole) const;

  /**
   * helper method: returns info data for data method
   *
   * \param index the index where the data should be written to
   * \param role one of several DisplayRoles for this column
   * \return returns the QVariant that is displayed as info
   */
  QVariant dataInfo(const QModelIndex &index, int role = Qt::DisplayRole) const;

  /**
   * Gets a MemoryValue at the given position either from the core or by using a
   * cache.
   * \param address Address of the MemoryValue
   * \param length Size of the MemoryValue
   * \return The requested MemoryValue
   */
  MemoryValue getMemoryValueCached(const std::size_t address,
                                   const std::size_t length) const;

  /**
   * Returns the number of rows in this table
   * Inherited from QAbstractListModel
   *
   * \param parent pointer to the logical data parent
   * \return returns the length of the table
   *
   */
  int rowCount(const QModelIndex &) const Q_DECL_OVERRIDE;

  /**
   * Returns the translation between roleNames in QML and the internal index
   * representation inherited from QAbstractListModel.
   *
   * \return returns a QHash with the connection between the internal index
   * representation and the name of the role in QML
   */
  QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

  /**
   * Turns a role string into the data format string by removing the number of
   * bits at the end.
   *
   * \returns The data format string.
   */
  static QString _roleToDataFormat(const QString &role);

  /** Holds a MemoryAccess for accessing the memory */
  MemoryAccess _memoryAccess;

  /** Holds a MemoryManager that handles the registration for callback functions
   */
  MemoryManager _memoryManager;

  /** Saves the size of the memory, as calling MemoryAccess::getMemorySize() in
   * rowCount causes a deadlock. */
  std::size_t _memorySize;

  /**
   * Cache for core memory on updates. When the memory is updates every view
   * has to fetch values multiple times. In order to decrease the number of
   * calls to the core this cache has been introduced.
   */
  mutable MemoryValue _memoryCache;

  /**
   * Size of core memory that is hold in cache.
   */
  mutable std::size_t _memoryCacheSize = 0;

  /**
   * Starting address of the memory hold in cache.
   * Just one block of memory will be kept in the cache.
   */
  mutable std::size_t _memoryCacheBaseAddress = 0;

  /**
   * Determines whether the current cache is valid or outdated.
   */
  mutable bool _memoryCacheValid = false;

  /** enumeration of all roles of the columns */
  enum ColumnRoles {
    AddressRole = Qt::UserRole,  // avoid collisions with predefined roles
    ValueRoleBin8,
    ValueRoleBin16,
    ValueRoleBin32,
    ValueRoleBin64,
    ValueRoleHex8,
    ValueRoleHex16,
    ValueRoleHex32,
    ValueRoleHex64,
    ValueRoleDec8,
    ValueRoleDec16,
    ValueRoleDec32,
    ValueRoleDec64,
    ValueRoleDecS8,
    ValueRoleDecS16,
    ValueRoleDecS32,
    ValueRoleDecS64,
    InfoRole
  };

 public slots:
  /**
   * Callback function for the core memory. Is beeing called when something in
   * the memory changes.
   *
   * \param address the address of the first cell with a new value
   * \param length the number of cells that were changed
   *
   */
  void onMemoryChanged(const std::size_t address, const std::size_t length);
};

#endif /* ERAGPSIM_UI_MEMORY_COMPONENT_PRESENTER_HPP */
