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

#include "arch/common/unit-container.hpp"
#include "common/string-conversions.hpp"
#include "core/architecture-access.hpp"
#include "core/memory-manager.hpp"
#include "core/memory-value.hpp"

RegisterModel::RegisterModel(ArchitectureAccess &architectureAccess,
                             MemoryManager &memoryManager,
                             MemoryAccess &memoryAccess,
                             QQmlContext *projectContext,
                             QObject *parent)
: QAbstractItemModel(parent)
, _rootItem(new RegisterInformation())
, _memoryAccess(memoryAccess) {
  projectContext->setContextProperty("registerModel", this);

  std::function<void(const std::string &)> callback = [this](
      const std::string &registerTitle) { this->updateContent(registerTitle); };
  memoryManager.setUpdateRegisterCallback(callback);

  // Fetch register units from core.
  UnitContainer registerUnits = architectureAccess.getRegisterUnits().get();
  // Iterate over each container and add the corresponding registers.
  for (auto unit : registerUnits) {
    // Add all normal registers to the model.
    for (auto it = unit.cbegin(); it != unit.cend(); ++it) {
      // Only add first-level registers to the dummy root item. Other registers
      // are referenced by their respective enclosing register.
      if (!it->second.hasEnclosing()) {
        _rootItem->addConstituent(
            ConstituentInformation(it->second.getID(), 0));
      }
      // Add every register to map of all registers, organised by their
      // identifier.
      _items.insert(std::pair<id_t, std::unique_ptr<RegisterInformation>>(
          it->first,
          std::unique_ptr<RegisterInformation>(
              new RegisterInformation(it->second))));
    }
    // Add all special registers to the model.
    for (auto it = unit.getSpecialRegisters().cbegin();
         it != unit.getSpecialRegisters().cend();
         ++it) {
      // Only add first-level registers to the dummy root item. Other registers
      // are referenced by their respective enclosing register.
      if (!it->second.hasEnclosing()) {
        _rootItem->addConstituent(
            ConstituentInformation(it->second.getID(), 0));
      }
      // Add every register to map of all registers, organised by their
      // identifier.
      _items.insert(std::pair<id_t, std::unique_ptr<RegisterInformation>>(
          it->second.getID(),
          std::unique_ptr<RegisterInformation>(
              new RegisterInformation(it->second))));
    }
  }
}


void RegisterModel::updateContent(const std::string &registerTitle) {
  // Notify the model about the change
  // Notifying requires the model index of the altered item.
  // The only reasonable way of accomplishing this is by filtering the registers
  // by their title.
  QModelIndexList alteredItems =
      match(this->index(0, 0),
            TitleRole,
            QString::fromStdString(registerTitle),
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

  if (role == TitleRole) {
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


void RegisterModel::registerContentChanged(
    const QModelIndex &index,
    const QString &registerContent,
    unsigned int currentDataFormatIndex) {
  RegisterInformation *registerItem =
      static_cast<RegisterInformation *>(index.internalPointer());
  // Remove whitespaces.
  QString registerContentCleared = registerContent;
  registerContentCleared.remove(QChar(' '));
  // Convert content string to MemoryValue.
  Optional<MemoryValue> registerContentMemoryValue;
  // Look up the register's current data format.
  Optional<QString> dataFormat =
      _dataFormatForRegisterItem(*registerItem, currentDataFormatIndex);
  if (dataFormat) {
    if (*dataFormat == "Binary") {
      registerContentMemoryValue = StringConversions::binStringToMemoryValue(
          registerContentCleared.toStdString(), registerItem->getSize());
    } else if (*dataFormat == "Hexadecimal") {
      registerContentMemoryValue = StringConversions::hexStringToMemoryValue(
          registerContentCleared.toStdString(), registerItem->getSize());
    } else if (*dataFormat == "Decimal (Unsigned)") {
      registerContentMemoryValue =
          StringConversions::unsignedDecStringToMemoryValue(
              registerContentCleared.toStdString(), registerItem->getSize());
    } else if (*dataFormat == "Decimal (Signed)") {
      registerContentMemoryValue =
          StringConversions::signedDecStringToMemoryValue(
              registerContentCleared.toStdString(), registerItem->getSize());
    } else if (*dataFormat == "Flag") {
      registerContentMemoryValue = StringConversions::binStringToMemoryValue(
          registerContentCleared.toStdString(), registerItem->getSize());
    }
  }
  if (registerContentMemoryValue) {
    _memoryAccess.setRegisterValue(registerItem->getName(),
                                   *registerContentMemoryValue);
  } else {
    _memoryAccess.setRegisterValue(registerItem->getName(),
                                   MemoryValue(registerItem->getSize()));
  }
  // Notify core.
  // TODO...
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


QVariant
RegisterModel::contentStringForRegister(const QModelIndex &index,
                                        unsigned int currentDataFormatIndex) {
  if (index.internalPointer() != nullptr) {
    // Load the register corresponding to the given QModelIndex.
    RegisterInformation *registerItem =
        static_cast<RegisterInformation *>(index.internalPointer());
    // Look up the register's current data format.
    Optional<QString> dataFormat =
        _dataFormatForRegisterItem(*registerItem, currentDataFormatIndex);
    // Fetch memory value from core.
    MemoryValue registerContentMemoryValue =
        _memoryAccess.getRegisterValue(registerItem->getName()).get();
    if (dataFormat) {
      // Return placeholder values.
      if (*dataFormat == "Binary") {
        return QString::fromStdString(
            StringConversions::toBinString(registerContentMemoryValue));
      } else if (*dataFormat == "Hexadecimal") {
        return QString::fromStdString(
            StringConversions::toHexString(registerContentMemoryValue));
      } else if (*dataFormat == "Decimal (Unsigned)") {
        return QString::fromStdString(
            StringConversions::toUnsignedDecString(registerContentMemoryValue));
      } else if (*dataFormat == "Decimal (Signed)") {
        return QString::fromStdString(
            StringConversions::toSignedDecString(registerContentMemoryValue));
      } else if (*dataFormat == "Flag") {
        return registerContentMemoryValue.get(0);
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
        _dataFormatForRegisterItem(*registerItem, currentDataFormatIndex);

    if (dataFormat) {
      // TODO: Get actual byte size
      return _computeDisplayFormatString(
          *dataFormat, registerItem->getSize(), 8);
    }
  }
  return "";
}


Optional<QString> RegisterModel::_dataFormatForRegisterItem(
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


QString RegisterModel::_computeDisplayFormatString(const QString &dataFormat,
                                                   size_t size,
                                                   size_t byteSize) const {
  // An empty format string means no format constraint is required.
  QString formatString;
  QString formatPrefix;
  size_t byteCharacterWidth = 0;
  // Compute format string depending on the amount of digits for the current
  // format.
  if (dataFormat == "Binary") {
    formatPrefix = "\\0\\b ";
    formatString = formatPrefix + QString(size, 'B');
    byteCharacterWidth = 8;
  } else if (dataFormat == "Hexadecimal") {
    formatPrefix = "\\0\\x ";
    formatString = formatPrefix + QString(size / 4, 'H');
    byteCharacterWidth = 2;
  }
  // Insert spaces after each byte.
  for (size_t bitIndex = formatPrefix.length() + byteCharacterWidth;
       bitIndex < formatString.length();
       bitIndex += (byteCharacterWidth + 1)) {
    formatString.insert(bitIndex, ' ');
  }
  return formatString;
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
    // Rows are specified by integery objects, therefore an integer value is
    // returned.
    return Optional<int>(
        std::distance(parentItem->getConstituents().begin(), it));
  }
  return Optional<int>();
}
