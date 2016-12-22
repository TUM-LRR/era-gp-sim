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
  explicit MemoryComponentPresenter(MemoryAccess access,
                                    MemoryManager manager,
                                    QQmlContext *projectContext,
                                    QObject *parent = 0);
  ~MemoryComponentPresenter();

  /**
   * Converts a hexademcimal representation of a string into a memory value
   * and saves it to the internal memory object.
   *
   * /param address the address of the cell to be updated
   * /param newvalue the new value for the memory cell
   * /param length_bit the number of bits shown in the string
   * /param presentation which numeric representation is used (eg. hex, oct, bin)
   *
   */
  Q_INVOKABLE void setValue(int address, QString newvalue, int length_bit, QString presentation);

  /**
   * Sets the context information for memory cells (NOT IMPLEMENTED YET)
   *
   * /param addressStart the starting address of the memory cell the context
   * information is related to
   * /param length the number of memory cells the context information is related
   * to
   * /param identifier the unique identifier for this context information (for
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
   * /param index the index where the data should be written to
   * /param role one of several DisplayRoles for this column
   * /return returns the QVariant that is displayed in the view
   *
   */
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

  /**
   * Returns the number of rows in this table
   * Inherited from QAbstractListModel
   *
   * /param parent pointer to the logical data parent
   * /return returns the length of the table
   *
   */
  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
   * Returns the translation between roleNames in QML and the internal index
   * representation inherited from QAbstractListModel.
   *
   * /return returns a QHash with the connection between the internal index
   * representation and the name of the role in QML
   */
  QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

  /**
   * Returns the number of bytes to be displayed in one memory cell.
   * The role is an indicator of how many bytes are chosen by the user.
   *
   * /param role the display role for this cell in the QAbstractListModel
   * /return the number of bytes to be shown in one cell
   */
  int numberOfBytes(int role) const;

  /** Holds a MemoryAccess for accessing the memory */
  MemoryAccess _memoryAccess;

  /** Holds a MemoryManager that handles the registration for callback functions
   */
  MemoryManager _memoryManager;


  /** Saves the size of the memory, as calling MemoryAccess::getMemorySize() in
   * rowCount causes a deadlock. */
  std::size_t _memorySize;

  /** enumeration of all roles of the columns */
  enum ColumnRoles {
      AddressRole8 = Qt::UserRole,// avoid collisions with predefined roles
      AddressRole16,
      AddressRole32,
      ValueRoleBin8,
      ValueRoleBin16,
      ValueRoleBin32,
      ValueRoleOct8,
      ValueRoleOct16,
      ValueRoleOct32,
      ValueRoleHex8,
      ValueRoleHex16,
      ValueRoleHex32,
      ValueRoleDec8,
      ValueRoleDec16,
      ValueRoleDec32,
      ValueRoleDecS8,
      ValueRoleDecS16,
      ValueRoleDecS32,
      InfoRole
  };

 public slots:
  /**
   * Callback function for the core memory. Is beeing called when something in
   * the memory changes.
   *
   * /param address the address of the first cell with a new value
   * /param length the number of cells that were changed
   *
   */
  void onMemoryChanged(const std::size_t address, const std::size_t length);
};

#endif /* ERAGPSIM_UI_MEMORY_COMPONENT_PRESENTER_HPP */
