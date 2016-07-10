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

#include "ui/registermodel.hpp"

RegisterModel::RegisterModel(const QString &data, QObject *parent)
: QAbstractItemModel(parent) {
  // The dummy root item can be used to save header information for each column.
  RegisterData rootData("Title", "Content", "DisplayFormatString");
  _rootItem = new RegisterItem(rootData);
  _setupModelData(data.split(QString("\n")), _rootItem);
}


RegisterModel::~RegisterModel() {
  delete _rootItem;
}


QHash<int, QByteArray> RegisterModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[TitleRole]               = "RegisterTitle";
  roles[ContentRole]             = "RegisterContent";
  roles[DisplayFormatStringRole] = "RegisterDisplayFormatString";
  return roles;
}


QVariant RegisterModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  // Return the row's correponding RegisterItem's data.
  RegisterData data =
      static_cast<RegisterItem *>(index.internalPointer())->getData();
  switch (role) {
    case TitleRole: return data.getTitle();
    case ContentRole: return data.getContent();
    case DisplayFormatStringRole: return data.getDisplayFormatString();
  }
  return QVariant();
}


Qt::ItemFlags RegisterModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) {
    return 0;
  }
  // Tell any views that this model is read-only.
  return QAbstractItemModel::flags(index);
}


QVariant RegisterModel::headerData(int section,
                                   Qt::Orientation orientation,
                                   int role) const {
  // Header data is stored inside the root item for convenience.
  RegisterData rootData = _rootItem->getData();
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case TitleRole: return rootData.getTitle();
      case ContentRole: return rootData.getContent();
      case DisplayFormatStringRole: return rootData.getDisplayFormatString();
    }
  }
  return QVariant();
}


QModelIndex
RegisterModel::index(int row, int column, const QModelIndex &parent) const {
  // Check if a valid index is being referred to. If not, return an invalid
  // index.
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  // Load the parent item. If no valid parent is specified, assume the rootItem
  // is being inferred to.
  RegisterItem *parentItem;
  if (!parent.isValid()) {
    parentItem = _rootItem;
  } else {
    parentItem = static_cast<RegisterItem *>(parent.internalPointer());
  }

  // Return the index for the parent's cell at the given row and column. If such
  // cell does not exist, return an invalid index.
  RegisterItem *childItem = parentItem->getChild(row);
  if (childItem) {
    return createIndex(row, column, childItem);
  } else {
    return QModelIndex();
  }
}


QModelIndex RegisterModel::parent(const QModelIndex &index) const {
  // If the given index is invalid, return an invalid parent index.
  if (!index.isValid()) {
    return QModelIndex();
  }
  // Load the child items and the corresponding parent item.
  RegisterItem *childItem =
      static_cast<RegisterItem *>(index.internalPointer());
  RegisterItem *parentItem = childItem->getParentItem();
  // The rootItem should never be referenced (it's a dummy item), therefore an
  // invalid index is returned in this case.
  if (parentItem == _rootItem) {
    return QModelIndex();
  }
  // When creating an index, the required parent's row and column within its one
  // parent must be specified. The row can be
  // retrieved from the item itself whereas the column is set to 0 by default
  // (any nested structure therefore occurs inside the
  // first column.
  return createIndex(parentItem->getRow(), 0, parentItem);
}


int RegisterModel::rowCount(const QModelIndex &parent) const {
  RegisterItem *parentItem;
  // As specified in RegisterMode::parent, any nested structure occurs inside
  // the first column. Therefore, any other column
  // does not have any rows.
  if (parent.column() > 0) {
    return 0;
  }

  if (!parent.isValid()) {
    parentItem = _rootItem;
  } else {
    parentItem = static_cast<RegisterItem *>(parent.internalPointer());
  }
  return parentItem->childCount();
}


int RegisterModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return static_cast<RegisterItem *>(parent.internalPointer())->columnCount();
  } else {// If no parent is specified, assume the rootItem is being referred
          // to.
    return _rootItem->columnCount();
  }
}


void RegisterModel::_setupModelData(const QStringList &lines,
                                    RegisterItem *parent) {
  // In the following method this list represents a stack containing the layers
  // (parents) for each item. If a new indentation occurs,
  // an item is pushed to this stack in order to be able to add child items to
  // it, which are defined in later lines.
  QList<RegisterItem *> parents;
  parents << parent;

  QList<int> indentations;
  indentations << 0;

  for (int lineIndex = 0; lineIndex < lines.count(); ++lineIndex) {
    int characterIndex = 0;
    // Counts the number of whitespaces and cuts the off the string.
    for (; characterIndex < lines[lineIndex].length(); ++characterIndex) {
      if (lines[lineIndex].at(characterIndex) != ' ') {
        break;
      }
    }
    QString lineData =
        lines[lineIndex].mid(characterIndex).trimmed();// Removes any
                                                       // whitespaces from the
                                                       // start and the end of
                                                       // the string.

    if (!lineData.isEmpty()) {
      // The rest of the string contains the information for each column of the
      // current row.
      QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
      QList<QString> columnData;
      for (int columnIndex = 0; columnIndex < columnStrings.count();
           ++columnIndex) {
        columnData << columnStrings[columnIndex];
      }
      RegisterData registerData(columnData[0], columnData[1], columnData[2]);

      // If the current line content was indented further than the previous line
      // content, the new data is supposed
      // to be the child of the previous data item.
      if (characterIndex > indentations.last()) {
        // A new parent is only required if the previous one already contains
        // any child items.
        if (parents.last()->childCount() > 0) {
          parents << parents.last()->getChild(parents.last()->childCount() - 1);
          indentations << characterIndex;
        }
      } else {
        // If the current indentation is shorter than the previous one, move
        // upwards on the layer stack until the
        // correct parent for this indentation is found.
        while (characterIndex < indentations.last() && parents.count() > 0) {
          parents.pop_back();
          indentations.pop_back();
        }
      }
      // Append a new item to the current parent's list of children.
      parents.last()->appendChild(
          new RegisterItem(registerData, parents.last()));
    }
  }
}
