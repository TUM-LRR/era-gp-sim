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

#ifndef ERAGPSIM_UI_REGISTERMODEL_HPP
#define ERAGPSIM_UI_REGISTERMODEL_HPP

#include <QAbstractItemModel>
#include <QQmlContext>
#include <map>
#include <string>

#include "arch/common/register-information.hpp"
#include "common/optional.hpp"
#include "core/memory-access.hpp"

class ArchitectureAccess;
class MemoryManager;

/**
 * Aggregates the data of multiple registers. Subclasses QAbstractItemModel to
 * be usable with QML-components.
 *
 * QAbstractItemModel is a table with each cell being referenced by row and
 * column. For RegisterModel, each row is represented by a RegisterInformation
 * object containing the register's constant data (e.g. title, constituents,
 * ...).
 * The RegisterModel only uses a single column containing the register itself.
 * Data that won't change over the lifetime of the model (e.g. register's title)
 * is stored locally within the model inside the RegisterInformation objects.
 * Dynamic data (i.e. register's content) is fetched from core when requested
 * (see data method).
 *
 * Registers of all levels are being held inside the _items-pointer-map where
 * they are identified by a unique register identifier. These pointers are
 * required in order to be able to pass them to the corresponding QModelIndex
 * (refer to index-method). Nesting is realised by specifying a parent register
 * identifier and possibly several child register identifiers inside the
 * RegisterInformation obejcts.
 * The _rootItem is used as a top-level dummy register not holding any actual
 * data but just referencing the visible top-level registers.
 */
class RegisterModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  using id_t = RegisterInformation::id_t;

  explicit RegisterModel(ArchitectureAccess &architectureAccess,
                         MemoryManager &memoryManager,
                         MemoryAccess &memoryAccess,
                         QQmlContext *projectContext,
                         QObject *parent = nullptr);

  /**
   * RegisterModelRole enum Identifies different kinds of data stored
   * for each register.
   *
   * In particular, there is one data role for each available data format type
   * (e.g. binary, hexadecimal etc.). A particular register selects the
   * corresponding content role depending on which format it should display.
   */
  enum RegisterModelRole {
    TitleRole,
    TypeRole,
    IsConstantRole,
    BinaryDataRole,
    HexDataRole,
    SignedDecimalDataRole,
    UnsignedDecimalDataRole,
    FlagDataRole
  };

  /**
   * Returns a QModelIndex for the specified item inside the
   * model.
   *
   * A model must provide an index to each item (i.e. register) it contains.
   * QModelIndexes provide a common interface for delegates to access data
   * inside a model independent of its internal structure. Each index contains
   * the item's parent, its row and column relative to its parent and a pointer
   * to the item it refers to. In this case, the item is a RegisterInformation
   * object.
   *
   * \param row The item's row relative to its parent item.
   * \param column The item's column.
   * \param parent The item's parent item.
   */
  QModelIndex
  index(int row,
        int column,
        const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
   * Returns the available role names of this model, which
   * the delegate (refer to corresponding QML delegate) can bind to.
   *
   * Data roles represent different types of representations of an item.
   * They can be used to differentiate between several parts of an item's
   * data.
   */
  QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

  /**
   * The data for the register referenced by the given
   * QModelIndex and specified by the given data role.
   *
   * RegisterModel offers role for the register's title, its type and each
   * available content format (e.g. binary, hexadecimal etc.). Depending
   * on its state, a register's content view (e.g. a certain text field)
   * can use one of the format roles to derive its content from.
   *
   * \param index The QModelIndex which points to the register item whose
   * data is requested.
   * \param role The data role specifying the requested type of data (e.g.
   * content, title).
   * \return The QML-compatable piece of data (e.g. QString).
   */
  QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

  /**
   * Returns an index refering to the register item's parent
   * register, if available.
   *
   * \param index The QModelIndex of the register item whose parent register
   * item is requested.
   */
  QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

  /**
   * The number of rows a given register item contains.
   * \param parent The register item's whose number of rows is requested.
   */
  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
   * The number of columns for each register item. Set
   * to 1 for this model as only one column containing the register itself
   * is available.
   * \param parent The parent register item whose children's column count is
   * requested.
   */
  int columnCount(const QModelIndex &parent = QModelIndex()) const
      Q_DECL_OVERRIDE;

  /**
   * Notifies the Core when a register's content
   * was changed by the user.
   * \param index The register's index.
   * \param registerContent The register's new content value.
   * \param currentDataFormatIndex The register's currently selected data
   * format.
   */
  Q_INVOKABLE void
  registerContentChanged(const QModelIndex &index,
                         QString registerContent,
                         const QString &currentDataFormatIndex);


 private:
  /// The dummy top-level item holding references to the visible top-level
  /// registers.
  std::unique_ptr<RegisterInformation> _rootItem;

  /// Map of all registers each identified by a unique register identifier.
  std::map<id_t, std::unique_ptr<RegisterInformation>> _items;

  /// Interface for communicating register data changes to core.
  MemoryAccess _memoryAccess;

  /**
   * Returns the row of a given register relative
   * to its parent register.
   * \param registerItem The register whose row is requested.
   * \return The row of the given register relative to its parent register.
   */
  Optional<int>
  _getRowRelativeToParent(RegisterInformation &registerItem) const;


 public slots:
  /**
   * Sets the content (i.e. AB01DE23) of a specified
   * register.
   * \param registerTitle The unique title of the register whose
   * content shall be altered.
   * \param registerContent The new register value.
   */
  void updateContent(const QString &registerTitle);
};

#endif  // ERAGPSIM_UI_REGISTERMODEL_HPP
