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

#include "ui/register-model.hpp"

RegisterModel::RegisterModel(QObject *parent)
: QAbstractItemModel(parent), _rootItem(new RegisterInformation()) {
  // TODO: Retrieve available registers from core.
  // Some temporary test registers.
  RegisterInformation *eax = new RegisterInformation("EAX");
  eax->type(RegisterInformation::Type::INTEGER);
  eax->size(32);
  _items.insert(std::pair<id_t, std::unique_ptr<RegisterInformation>>(
      eax->getID(), std::unique_ptr<RegisterInformation>(eax)));
  _rootItem->addConstituent(ConstituentInformation(eax->getID(), 0));
  RegisterInformation *ax = new RegisterInformation("AX");
  ax->type(RegisterInformation::Type::INTEGER);
  ax->size(16);
  ax->enclosing(eax->getID());
  _items.insert(std::pair<id_t, std::unique_ptr<RegisterInformation>>(
      ax->getID(), std::unique_ptr<RegisterInformation>(ax)));
  eax->addConstituent(ConstituentInformation(ax->getID(), 0));
  // Status-Register
  RegisterInformation *statusReg = new RegisterInformation("Statusregister");
  statusReg->type(RegisterInformation::Type::INTEGER);
  statusReg->size(8);
  _items.insert(std::pair<id_t, std::unique_ptr<RegisterInformation>>(
      statusReg->getID(), std::unique_ptr<RegisterInformation>(statusReg)));
  _rootItem->addConstituent(ConstituentInformation(statusReg->getID(), 0));
  // Status-Register, Flag 1
  RegisterInformation *flag1 = new RegisterInformation("Sign");
  flag1->type(RegisterInformation::Type::FLAG);
  flag1->size(1);
  flag1->enclosing(statusReg->getID());
  _items.insert(std::pair<id_t, std::unique_ptr<RegisterInformation>>(
      flag1->getID(), std::unique_ptr<RegisterInformation>(flag1)));
  statusReg->addConstituent(ConstituentInformation(flag1->getID(), 0));
  // Status-Register, Flag 2
  RegisterInformation *flag2 = new RegisterInformation("Zero");
  flag2->type(RegisterInformation::Type::FLAG);
  flag2->size(1);
  flag2->enclosing(statusReg->getID());
  _items.insert(std::pair<id_t, std::unique_ptr<RegisterInformation>>(
      flag2->getID(), std::unique_ptr<RegisterInformation>(flag2)));
  statusReg->addConstituent(ConstituentInformation(flag2->getID(), 1));
}


void RegisterModel::updateContent(id_t registerIdentifier) {
  RegisterInformation *registerItem = _items[registerIdentifier].get();
  // Notify the model about the change
  // Notifying requires the model index of the altered item.
  // The only reasonable way of accomplishing this is by filtering the registers
  // by their title.
  QModelIndexList alteredItems =
      match(this->index(0, 0),
            TitleRole,
            QString::fromStdString(registerItem->getName()),
            1,
            Qt::MatchFlags(Qt::MatchExactly | Qt::MatchRecursive));
  if (!alteredItems.isEmpty()) {
    qDebug("found");
    QModelIndex alteredItem = alteredItems.first();
    // The data method will be called, fetching the updated content.
    emit dataChanged(alteredItem, alteredItem);
  }
}


QHash<int, QByteArray> RegisterModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[IdentifierRole]      = "Identifier";
  roles[TitleRole]           = "Title";
  roles[DataFormatsListRole] = "DataFormatsList";
  return roles;
}


QVariant RegisterModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  // Return the row's correponding RegisterItem's information.
  RegisterInformation *information =
      static_cast<RegisterInformation *>(index.internalPointer());

  if (role == IdentifierRole) {
    QVariant identifier;
    identifier.setValue(information->getID());
    return identifier;
  } else if (role == TitleRole) {
    return QString::fromStdString(information->getName());
  } else if (role == DataFormatsListRole) {
    return _dataFormatLists.at(information->getType());
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
  id_t childItemIdentifier = parentItem->getConstituents().at(row).getID();
  auto it                  = _items.find(childItemIdentifier);
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
  RegisterInformation *childItem =
      static_cast<RegisterInformation *>(index.internalPointer());
  // If the child item has no parent item, return an invalid parent index.
  if (childItem->hasEnclosing()) {
    // Load the requested parent item.
    id_t parentItemIdentifier = childItem->getEnclosing();
    auto it                   = _items.find(parentItemIdentifier);
    if (it != _items.end()) {
      RegisterInformation *parentItem = (it->second).get();

      // Determine the parentItem's row among the child items of its own parent.
      auto row = getRowRelativeToParent(*parentItem);
      if (row) {
        return createIndex(*row, 0, parentItem);
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


void RegisterModel::registerContentChanged(const QString &registerContent) {
  qDebug() << "registerContentChanged: " << registerContent;
  // TODO: Convert to MemoryValue.
  // TODO: Notify Core.
}


QStringList
RegisterModel::dataFormatListForRegister(const QModelIndex &index) const {
  RegisterInformation *registerItem =
      static_cast<RegisterInformation *>(index.internalPointer());
  if (registerItem != nullptr) {
    return _dataFormatLists.at(registerItem->getType());
  }
  return QStringList();
}


QVariant RegisterModel::contentStringForRegister(
    const QModelIndex &index, unsigned int currentDataFormatIndex) const {
  // TODO: Fetch memory value from core.
  // TODO: Convert memory value to string with format as specified by
  // currentDataFormatIndex.
  if (index.internalPointer() != nullptr) {
    RegisterInformation *registerItem =
        static_cast<RegisterInformation *>(index.internalPointer());
    Optional<QString> dataFormat =
        dataFormatForRegisterItem(*registerItem, currentDataFormatIndex);
    if (dataFormat) {
      // Return placeholder values.
      if (*dataFormat == "Binary") {
        return "10101011000000011100110100100011";
      } else if (*dataFormat == "Hexadecimal") {
        return "AB 01 CD 23";
      } else if (*dataFormat == "Decimal (Unsigned)") {
        return "2869021987";
      } else if (*dataFormat == "Decimal (Signed)") {
        return "-1425945309";
      } else if (*dataFormat == "Flag") {
        return QVariant(false);
      }
    }
  }
  return "";
}


QString RegisterModel::displayFormatStringForRegister(
    const QModelIndex &index, unsigned int currentDataFormatIndex) const {
  if (index.internalPointer() != nullptr) {
    RegisterInformation *registerItem =
        static_cast<RegisterInformation *>(index.internalPointer());
    Optional<QString> dataFormat =
        dataFormatForRegisterItem(*registerItem, currentDataFormatIndex);

    if (dataFormat) {
      // TODO: Get actual byte size
      return computeDisplayFormatString(
          *dataFormat, registerItem->getSize(), 8);
    }
  }
  return "";
}


Optional<QString> RegisterModel::dataFormatForRegisterItem(
    const RegisterInformation &registerItem,
    unsigned int currentDataFormatIndex) const {
  QStringList dataFormats = _dataFormatLists.at(registerItem.getType());
  // Sometimes a view requests data before the dataFormatCombobox was
  // initialized and therefore sends an invalid value for
  // currentDataFormatIndex. This has to be handled.
  if (currentDataFormatIndex < dataFormats.length()) {
    return dataFormats.at(currentDataFormatIndex);
  }
  return Optional<QString>();
}


QString RegisterModel::computeDisplayFormatString(const QString &dataFormat,
                                                  size_t size,
                                                  size_t byteSize) const {
  // An empty format string means no format constraint is required.
  QString formatString = "";
  // Compute format string depending on the amount of digits for the current
  // format.
  if (dataFormat == "Binary") {
    for (int byteIndex = 0; byteIndex < (size / byteSize); byteIndex++) {
      formatString.append("BBBBBBBB ");
    }
  } else if (dataFormat == "Hexadecimal") {
    formatString = "\\0\\x> ";
    for (int byteIndex = 0; byteIndex < (size / byteSize); byteIndex++) {
      formatString.append("HH ");
    }
  }
  return formatString;
}


Optional<int>
RegisterModel::getRowRelativeToParent(RegisterInformation &registerItem) const {
  // Determine the parent register.
  RegisterInformation *parentItem;
  if (registerItem.hasEnclosing()) {
    parentItem = (_items.find(registerItem.getEnclosing())->second).get();
  } else {
    parentItem = _rootItem.get();
  }

  // Determine the index of the registerItem inside the parent register's
  // constituents.
  auto it = parentItem->getConstituents().begin();
  for (; it != parentItem->getConstituents().end(); ++it) {
    if (it->getID() == registerItem.getID()) {
      break;
    }
  }

  // Return the index if it was found.
  if (it != parentItem->getConstituents().end()) {
    // Rows are specified by integery objects, therefore an integer value is
    // returned.
    return Optional<int>(
        std::distance(parentItem->getConstituents().begin(), it));
  }
  return Optional<int>();
}
