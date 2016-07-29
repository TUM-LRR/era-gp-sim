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
 * data
 * but just referencing the visible top-level registers.
 */
class RegisterModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  explicit RegisterModel(QObject *parent = 0);

  enum RegisterModelRole {
    TitleRole,
    ContentRole,
    DisplayFormatStringRole,
    DataFormatsListRole
  };

  QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
  QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
  QModelIndex
  index(int row,
        int column,
        const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
  QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
  int columnCount(const QModelIndex &parent = QModelIndex()) const
      Q_DECL_OVERRIDE;

 private:
  /// The dummy top-level item holding references to the visible top-level
  /// registers.
  std::unique_ptr<RegisterItem> _rootItem;
  /// Map of all registers identified by a unique register identifier.
  std::map<std::string, std::unique_ptr<RegisterItem>> _items;
};

#endif// REGISTERMODEL_H
