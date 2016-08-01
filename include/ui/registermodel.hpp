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

#ifndef REGISTERMODEL_H
#define REGISTERMODEL_H

#include <QAbstractItemModel>
#include <map>
#include <string>
#include "ui/registeritem.hpp"

/**
 * Aggregates the data of multiple registers. Subclasses QAbstractItemModel to
 * be usable with QML-components.
 *
 * QAbstractItemModel is a table with each cell being referenced by row and
 * column. For RegisterModel, each row is represented by a RegisterItem, which
 * contains a RegisterData object holding a single register's data (e.g. title,
 * content, ...). The RegisterModel only uses a single column containing the
 * register itself.
 *
 * Registers of all levels are being held inside the _items-map where they are
 * identified by a unique register identifier. Nesting is realised by specifying
 * a parent register identifier and possibly several child register identifiers.
 * The _rootItem is used as a top-level dummy register not holding any actual
 * data but just referencing the visible top-level registers.
 */
class RegisterModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  explicit RegisterModel(QObject *parent = 0);

  /// Identifies different kinds of data stored for each register.
  enum RegisterModelRole {
    TitleRole,
    ContentRole,
    DisplayFormatStringRole,
    DataFormatsListRole
  };

  /**
   * @brief index Returns a QModelIndex for the specified item inside the
   * model.
   * A model must provide an index to each item (i.e. register) it contains.
   * QModelIndexes provide a common interface for delegates to access data
   * inside a model independent of its internal structure. Each index contains
   * the item's parent, its row and column relative to its parent and a pointer
   * to the item it refers to.
   *
   * @param row The item's row relative to its parent item.
   * @param column The item's column.
   * @param parent The item's parent item.
   */
  QModelIndex
  index(int row,
        int column,
        const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
   * @brief roleNames Returns the available role names of this model, which
   * the delegate (refer to corresponding QML delegate) can bind to.
   *
   * Data roles represent different types of representations of an item.
   * They can be used to differentiate between several parts of an item's
   * data.
   */
  QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

  /**
   * @brief data The data for the register referenced by the given
   * QModelIndex and specified by the given data role.
   *
   * @param index The QModelIndex which points to the register item whose
   * data is requested.
   * @param role The data role specifying the requested type of data (e.g.
   * content, title).
   * @return The QML-compatable piece of data (e.g. QString, QStringList).
   */
  QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

  /**
   * @brief parent Returns an index refering to the register item's parent
   * register, if available.
   *
   * @param index The QModelIndex of the register item whose parent register
   * item is requested.
   */
  QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

  /**
   * @brief rowCount The number of rows a given register item contains.
   * @param parent The register item's whose number of rows is requested.
   */
  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
   * @brief columnCount The number of columns for each register item. Set
   * to 1 for this model as only one column containing the register itself
   * is available.
   * @param parent The parent register item whose children's column count is
   * requested.
   */
  int columnCount(const QModelIndex &parent = QModelIndex()) const
      Q_DECL_OVERRIDE;

 private:
  /// The dummy top-level item holding references to the visible top-level
  /// registers.
  std::unique_ptr<RegisterItem> _rootItem;
  /// Map of all registers each identified by a unique register identifier.
  std::map<std::string, std::unique_ptr<RegisterItem>> _items;
};

#endif// REGISTERMODEL_H
