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

#include <QByteArray>
#include <QString>
#include <map>

#include "arch/common/unit-container.hpp"
#include "common/string-conversions.hpp"
#include "core/architecture-access.hpp"
#include "core/memory-manager.hpp"
#include "core/memory-value.hpp"

std::map<QByteArray, RegisterModel::MemoryValueToStringConversion>
    RegisterModel::_memoryValueToStringConversions = {
        {"BinaryData", StringConversions::toBinString},
        {"HexData", StringConversions::toHexString},
        {"SignedDecData", StringConversions::toSignedDecString},
        {"UnsignedDecData", StringConversions::toUnsignedDecString}};

std::map<QString, RegisterModel::StringToMemoryValueConversion>
    RegisterModel::_stringToMemoryValueConversions = {
        {"Binary", StringConversions::binStringToMemoryValue},
        {"Hexadecimal", StringConversions::hexStringToMemoryValue},
        {"Signed Decimal", StringConversions::signedDecStringToMemoryValue},
        {"Unsigned Decimal",
         StringConversions::unsignedDecStringToMemoryValue}};


RegisterModel::RegisterModel(ArchitectureAccess &architectureAccess,
                             MemoryManager &memoryManager,
                             MemoryAccess &memoryAccess,
                             QQmlContext *projectContext,
                             QObject *parent)
: QAbstractItemModel(parent)
, _rootItem(new RegisterInformation())
, _memoryAccess(memoryAccess) {
  projectContext->setContextProperty("registerModel", this);

  // Fetch register units from core.
  UnitContainer registerUnits = architectureAccess.getRegisterUnits().get();
  // Iterate over each container and add the corresponding registers.
  for (auto unit : registerUnits) {
    const UnitInformation::SortedResult unitRegisters =
        unit.getAllRegisterSorted(UnitInformation::IdOrder());
    for (const RegisterInformation &registerItem : unitRegisters) {
      // Only add first-level registers to the dummy root item. Other registers
      // are referenced by their respective enclosing register.
      if (!registerItem.hasEnclosing()) {
        _rootItem->addConstituent(
            ConstituentInformation(registerItem.getID(), 0));
      }
      // Add every register to map of all registers, organised by their
      // identifier.
      _items.insert(std::pair<id_t, std::unique_ptr<RegisterInformation>>(
          registerItem.getID(),
          std::unique_ptr<RegisterInformation>(
              new RegisterInformation(registerItem))));
    }
  }
}


void RegisterModel::updateContent(const QString &registerTitle) {
  // Notify the model about the change
  // Notifying requires the model index of the altered item.
  // The only reasonable way of accomplishing this is by filtering the registers
  // by their title.
  QModelIndexList alteredItems =
      match(this->index(0, 0),
            TitleRole,
            registerTitle,
            1,
            Qt::MatchFlags(Qt::MatchExactly | Qt::MatchRecursive));
  if (!alteredItems.isEmpty()) {
    QModelIndex alteredItem = alteredItems.first();
    // The data method will be called, fetching the updated content.
    emit dataChanged(alteredItem, alteredItem);
  }
}


QHash<int, QByteArray> RegisterModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[TitleRole] = "Title";
  roles[TypeRole] = "Type";
  roles[IsConstantRole] = "IsConstant";
  roles[BinaryDataRole] = "BinaryData";
  roles[HexDataRole] = "HexData";
  roles[SignedDecDataRole] = "SignedDecData";
  roles[UnsignedDecDataRole] = "UnsignedDecData";
  roles[FlagDataRole] = "FlagData";
  return roles;
}


QVariant RegisterModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  // Return the row's correponding RegisterItem's information.
  RegisterInformation *registerItem =
      static_cast<RegisterInformation *>(index.internalPointer());

  switch (role) {
    case TitleRole: return QString::fromStdString(registerItem->getName());
    case TypeRole:
      switch (registerItem->getType()) {
        case RegisterInformation::Type::INTEGER: return "Integer";
        case RegisterInformation::Type::FLOAT: return "Float";
        case RegisterInformation::Type::VECTOR: return "Vector";
        case RegisterInformation::Type::FLAG: return "Flag";
        case RegisterInformation::Type::LINK: return "Link";
        case RegisterInformation::Type::PROGRAM_COUNTER:
          return "ProgramCounter";
      }
    case IsConstantRole: return registerItem->isConstant();
    case FlagDataRole:
      return _memoryAccess.getRegisterValue(registerItem->getName())
          .get()
          .get(0);
    default:
      auto registerValue =
          _memoryAccess.getRegisterValue(registerItem->getName()).get();
      return QString::fromStdString(
          _memoryValueToStringConversions.at(roleNames()[role])(registerValue));
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
  auto it = _items.find(childItemIdentifier);
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
    auto it = _items.find(parentItemIdentifier);
    if (it != _items.end()) {
      RegisterInformation *parentItem = (it->second).get();

      // Determine the parentItem's row among the child items of its own parent.
      auto row = _getRowRelativeToParent(*parentItem);
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
  if (parent.column() > 0) return 0;

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


void RegisterModel::registerContentChanged(const QModelIndex &index,
                                           QString registerContent,
                                           const QString &dataFormat) {
  auto registerItem =
      static_cast<RegisterInformation *>(index.internalPointer());

  auto content = registerContent.remove(QChar(' ')).toStdString();
  assert::that(_stringToMemoryValueConversions.count(dataFormat) > 0);
  auto converter = _stringToMemoryValueConversions[dataFormat];
  auto memory = converter(content, registerItem->getSize());

  // Notify core about the change
  if (memory) {
    _memoryAccess.setRegisterValue(registerItem->getName(), *memory);
  } else {
    MemoryValue empty(registerItem->getSize());
    _memoryAccess.setRegisterValue(registerItem->getName(), empty);
  }
}


Optional<int> RegisterModel::_getRowRelativeToParent(
    RegisterInformation &registerItem) const {
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
    return Optional<int>(
        std::distance(parentItem->getConstituents().begin(), it));
  }
  return Optional<int>();
}
