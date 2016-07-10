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

#ifndef REGISTERMODEL_H
#define REGISTERMODEL_H

#include <QAbstractItemModel>
#include <QDebug>
#include <QHash>
#include "ui/registeritem.hpp"

/**
 * Aggregates the data of multiple registers. Subclasses QAbstractItemModel to
 * be usable with QML-components.
 *
 * QAbstractItemModel as a table with each cell being referenced by row and
 * column. For RegisterModel, each row is
 * represented by an RegisterItem, which contains a RegisterData object holding
 * a single registers data (e.g. title, content, ...).
 *
 * The table can be nested by specifying one row as the parent of several other
 * rows. The parent row holds a reference
 * to all its child rows and each child row holds a reference to its parent row.
 * On its top level, the model holds a dummy row (rootItem) comprising all the
 * actual top level rows. This allows every cell to be
 * referenced by just holding a reference to the dummy row.
 */
class RegisterModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  explicit RegisterModel(const QString &data, QObject *parent = 0);
  ~RegisterModel();

  enum RegisterModelRole { TitleRole, ContentRole, DisplayFormatStringRole };

  QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
  QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
  QModelIndex
  index(int row,
        int column,
        const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
  QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
  int columnCount(const QModelIndex &parent = QModelIndex()) const
      Q_DECL_OVERRIDE;

 private:
  /**
   * Loads the model's content from a list of strings each containing a
   * description of the models' items.
   * @param lines A list of strings each containing a description of the models'
   * items.
   * @param parent Root item used as a reference point for the entire nested
   * model being built by this method.
   */
  void _setupModelData(const QStringList &lines, RegisterItem *parent);

  RegisterItem *_rootItem;
};

#endif// REGISTERMODEL_H
