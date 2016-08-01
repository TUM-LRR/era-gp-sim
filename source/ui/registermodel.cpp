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

#include "ui/registermodel.hpp"

RegisterModel::RegisterModel(QObject *parent)
: QAbstractItemModel(parent)
, _rootItem(
      new RegisterItem(RegisterData("Title", "Content", "DisplayFormatString"),
                       "",
                       std::vector<std::string>{"EAX", "EBX"})) {
  // Initializing some example registers.
  _items["EAX"] = std::unique_ptr<RegisterItem>(
      new RegisterItem(RegisterData("EAX",
                                    "AB CD 01 23",
                                    "HH HH HH HH",
                                    QStringList() << "Hex"
                                                  << "Dec"
                                                  << "Bin"
                                                  << "Vig"),
                       "",
                       std::vector<std::string>{"AX"}));
  _items["AX"] = std::unique_ptr<RegisterItem>(
      new RegisterItem(RegisterData("AX", "AB CD", "HH HH"),
                       "EAX",
                       std::vector<std::string>{"AL", "AH"}));
  _items["AL"] = std::unique_ptr<RegisterItem>(new RegisterItem(
      RegisterData("AL", "CD", "HH"), "AX", std::vector<std::string>()));
  _items["AH"] = std::unique_ptr<RegisterItem>(new RegisterItem(
      RegisterData("AH", "AB", "HH"), "AX", std::vector<std::string>()));
  _items["EBX"] = std::unique_ptr<RegisterItem>(
      new RegisterItem(RegisterData("EBX", "AB 45 CD 78", "HH HH HH HH"),
                       "",
                       std::vector<std::string>{"BX"}));
  _items["BX"] = std::unique_ptr<RegisterItem>(
      new RegisterItem(RegisterData("BX", "AB 45", "HH HH"),
                       "EBX",
                       std::vector<std::string>{"BL", "BH"}));
  _items["BL"] = std::unique_ptr<RegisterItem>(new RegisterItem(
      RegisterData("BL", "45", "HH"), "BX", std::vector<std::string>()));
  _items["BH"] = std::unique_ptr<RegisterItem>(new RegisterItem(
      RegisterData("BH", "AB", "HH"), "BX", std::vector<std::string>()));
}


QHash<int, QByteArray> RegisterModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[TitleRole]               = "RegisterTitle";
  roles[ContentRole]             = "RegisterContent";
  roles[DisplayFormatStringRole] = "RegisterDisplayFormatString";
  roles[DataFormatsListRole]     = "DataFormatsList";
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
    case DataFormatsListRole: return data.getDataFormatsList();
  }
  return QVariant();
}


QModelIndex
RegisterModel::index(int row, int column, const QModelIndex &parent) const {
  // Check if a valid item is being referred to. If not, return an invalid
  // index.
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  // Load the parent item. If no valid parent is specified, assume the rootItem
  // is being inferred to.
  RegisterItem *parentItem;
  if (!parent.isValid()) {
    parentItem = _rootItem.get();
  } else {
    parentItem = static_cast<RegisterItem *>(parent.internalPointer());
  }

  // Return the index for the parent's cell at the given row and column. If such
  // cell does not exist, return an invalid index.
  std::string childItemIdentifier = parentItem->getChildItemIdentifier(row);
  auto it                         = _items.find(childItemIdentifier);
  if (it != _items.end()) {
    RegisterItem *childItem = (it->second).get();
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
  // Load the child item the given QModelIndex refers to and derive the parent
  // item from it.
  RegisterItem *childItem =
      static_cast<RegisterItem *>(index.internalPointer());
  std::string parentItemIdentifier = childItem->getParentItemIdentifier();
  auto it                          = _items.find(parentItemIdentifier);
  if (it != _items.end()) {
    RegisterItem *parentItem = (it->second).get();
    // Determine the parentItem's row among the child items of its own parent.
    int row;
    if (parentItem->getParentItemIdentifier() != "") {
      RegisterItem *parentParentItem =
          (_items.find(parentItem->getParentItemIdentifier())->second).get();
      row = parentParentItem->getRowOfChild(parentItemIdentifier);
    } else {
      row = _rootItem->getRowOfChild(parentItemIdentifier);
    }
    // Return the QModelIndex referencing the parent item. Note that any nesting
    // is done inside the first column (column 0).
    return createIndex(row, 0, parentItem);
  }
  // If the parentItemIdentifier is invalid we can assume index referenced the
  // top-level dummy item and therefore return an invalid index.
  return QModelIndex();
}


int RegisterModel::rowCount(const QModelIndex &parent) const {
  RegisterItem *parentItem;
  // As specified in RegisterModel::parent, any nested structure occurs inside
  // the first column. Therefore, any other column
  // does not have any rows.
  if (parent.column() > 0) {
    return 0;
  }

  if (!parent.isValid()) {
    parentItem = _rootItem.get();
  } else {
    parentItem = static_cast<RegisterItem *>(parent.internalPointer());
  }
  return parentItem->childCount();
}


int RegisterModel::columnCount(const QModelIndex &parent) const {
  return 1;
}
