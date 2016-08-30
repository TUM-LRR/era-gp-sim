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

RegisterModel::RegisterModel(QObject *parent) : QAbstractItemModel(parent), _rootItem(new RegisterInformation()) {
    // TODO: Retrieve registers from core.
    // Some temporary test registers.
    RegisterInformation *eax = new RegisterInformation("EAX");
    eax->type(RegisterInformation::Type::INTEGER);
    _items.insert(std::pair<id_t, std::unique_ptr<RegisterInformation>>(eax->getID(), std::unique_ptr<RegisterInformation>(eax)));
    _rootItem->addConstituent(eax->getID());
    RegisterInformation *ax = new RegisterInformation("AX");
    eax->type(RegisterInformation::Type::INTEGER);
    _items.insert(std::pair<id_t, std::unique_ptr<RegisterInformation>>(ax->getID(), std::unique_ptr<RegisterInformation>(ax)));
    eax->addConstituent(ax->getID());
}


void RegisterModel::updateContent(id_t registerIdentifier) {
  RegisterInformation *registerItem = _items[registerIdentifier].get();
  // Notify the model about the change
  // Notifying requires the model index of the altered item.
  // The only reasonable way of accomplishing this is by filtering the registers by their title.
  QModelIndexList alteredItems =
      match(this->index(0, 0),
            TitleRole,
            QString::fromStdString(registerItem->getName()),
            1,
            Qt::MatchExactly);
  if (!alteredItems.isEmpty()) {
    QModelIndex alteredItem = alteredItems.first();
    // The data method will be called, fetching the updated content.
    dataChanged(alteredItem, alteredItem);
  }
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

  // Return the row's correponding RegisterItem's information.
  RegisterInformation *information =
      static_cast<RegisterInformation *>(index.internalPointer());
  switch (role) {
    case TitleRole: return QString::fromStdString(information->getName());
    // TODO: Retrieve actual data from core.
    case ContentRole: return "AB 01 CD 23";
    // TODO: Move to core.
    case DisplayFormatStringRole: return "HH HH HH HH";
    case DataFormatsListRole:
      switch (information->getType()) {
        case RegisterInformation::Type::INTEGER: return QStringList() << "Binary" << "Hex" << "Signed Integer" << "Unsigned Integer";
        case RegisterInformation::Type::FLOAT: return QStringList() << "Binary" << "Hex" << "Float";
        case RegisterInformation::Type::VECTOR: return QStringList() << "Binary" << "Hex" << "Vector";
        case RegisterInformation::Type::FLAG: return QStringList() << "Binary" << "Flag";
        case RegisterInformation::Type::LINK: return QStringList() << "Binary" << "Hex";
        case RegisterInformation::Type::PROGRAM_COUNTER: return QStringList() << "Binary" << "Hex";
      }
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
  // Get the requested register's parent register.
  RegisterInformation *parentItem;
  if (!parent.isValid()) {
      parentItem = _rootItem.get();
  } else {
      parentItem = static_cast<RegisterInformation *>(parent.internalPointer());
  }
  // Return the index for the parent's cell at the given row and column. If such
  // cell does not exist, return an invalid index.
  id_t childItemIdentifier = parentItem->getConstituents().at(row);
  auto it                         = _items.find(childItemIdentifier);
  if (it != _items.end()) {
    RegisterInformation *childItem = (it->second).get();
    return createIndex(row, column, childItem);
  }
  return QModelIndex();
}

QModelIndex RegisterModel::parent(const QModelIndex &index) const {
  // If the given index is invalid, return an invalid parent index.
  if (!index.isValid()) {
    return QModelIndex();
  }
  // Load the child item the given QModelIndex refers to.
  RegisterInformation *childItem = static_cast<RegisterInformation *>(index.internalPointer());
  // If the child item has no parent item, return an invalid parent index.
  if (childItem->hasEnclosing()) {
      // Load the requested parent item.
      id_t parentItemIdentifier = childItem->getEnclosing();
      auto it = _items.find(parentItemIdentifier);
      if (it != _items.end()) {
        RegisterInformation *parentItem = (it->second).get();
        // Determine the parentItem's row among the child items of its own parent.
        RegisterInformation *parentParentItem;
        if (parentItem->hasEnclosing()) {
          parentParentItem = (_items.find(parentItem->getEnclosing())->second).get();
        } else {
          parentParentItem = _rootItem.get();
        }
        auto it = std::find(parentParentItem->getConstituents().begin(),
                            parentParentItem->getConstituents().end(),
                            parentItemIdentifier);
        if (it != parentParentItem->getConstituents().end()) {
          int row = std::distance(parentParentItem->getConstituents().begin(), it);

          // Return the QModelIndex referencing the parent item. Note that any nesting
          // is done inside the first column (column 0).
          return createIndex(row, 0, parentItem);
        }
      }
  }
  return QModelIndex();
}


int RegisterModel::rowCount(const QModelIndex &parent) const {
  RegisterInformation *parentItem;
  // As specified in RegisterModel::parent, any nested structure occurs inside
  // the first column. Therefore, any other column
  // does not have any rows.
  if (parent.column() > 0) {
    return 0;
  }

  if (!parent.isValid()) {
    return _rootItem->getConstituents().size();
  } else {
    parentItem = static_cast<RegisterInformation *>(parent.internalPointer());
    return parentItem->getConstituents().size();
  }
  return 0;
}


int RegisterModel::columnCount(const QModelIndex &parent) const {
  return 1;
}
