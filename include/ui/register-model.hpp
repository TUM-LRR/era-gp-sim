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
#include <QDebug>
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
 * are stored within the model inside the RegisterInformation objects. Dynamic
 * data (i.e. register's content) is fetched from core when requested (see data-
 * method).
 *
 * Registers of all levels are being held inside the _items-map where they are
 * identified by a unique register identifier. Nesting is realised by specifying
 * a parent register identifier and possibly several child register identifiers
 * inside the RegisterInformation obejcts.
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
                         QObject *parent = 0);

  /**
   * \brief The RegisterModelRole enum Identifies different kinds of data stored
   * for each register.
   * There is no role for the register's content; refer to data method's
   * description for an explanation.
   */
  enum RegisterModelRole { TitleRole, DataFormatsListRole };

  /**
   * \brief index Returns a QModelIndex for the specified item inside the
   * model.
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
   * \brief roleNames Returns the available role names of this model, which
   * the delegate (refer to corresponding QML delegate) can bind to.
   *
   * Data roles represent different types of representations of an item.
   * They can be used to differentiate between several parts of an item's
   * data.
   */
  QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

  /**
   * \brief data The data for the register referenced by the given
   * QModelIndex and specified by the given data role.
   *
   * RegisterModel purposely has no role for the register's content and
   * therefore does not
   * expose it via its data-method. This is due to the fact that there might
   * be two views of the same RegisterModel visible on screen for which the user
   * can independently set the data format (e.g. EAX has Hex-format in first
   * view and Bin-format in second view). Even though the memory value is the
   * same for both presentations the actual content-string varies significantly
   * and can therefore not be fed by the same data-role. To achieve the desired
   * behavior, the register's content properties (such as text and input mask
   * for regular text fields) are connected (refer to QML-files
   * DefaultRegister.qml and similar) to methods responsible for fetching and
   * converting the required data (refer to `contentStringForRegister`or
   * `displayFormatStringForRegister`) whenever the corresponding component is
   * loaded or reloaded taking into consideration the currently selected format
   * of the exact view of the register.
   *
   * \param index The QModelIndex which points to the register item whose
   * data is requested.
   * \param role The data role specifying the requested type of data (e.g.
   * content, title).
   * \return The QML-compatable piece of data (e.g. QString, QStringList).
   */
  QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

  /**
   * \brief parent Returns an index refering to the register item's parent
   * register, if available.
   *
   * \param index The QModelIndex of the register item whose parent register
   * item is requested.
   */
  QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

  /**
   * \brief rowCount The number of rows a given register item contains.
   * \param parent The register item's whose number of rows is requested.
   */
  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
   * \brief columnCount The number of columns for each register item. Set
   * to 1 for this model as only one column containing the register itself
   * is available.
   * \param parent The parent register item whose children's column count is
   * requested.
   */
  int columnCount(const QModelIndex &parent = QModelIndex()) const
      Q_DECL_OVERRIDE;

  /**
   * \brief updateContent Sets the content (i.e. AB01DE23) of a specified
   * register.
   * \param registerTitle The unique title of the register whose
   * content shall be altered.
   * \param registerContent The new register value.
   */
  void updateContent(const std::string &registerTitle);

  /**
   * \brief registerContentChanged Notifies the Core when a register's content
   * was changed by the use.
   * \param registerContent The register's new content value.
   */
  Q_INVOKABLE void registerContentChanged(const QModelIndex &index,
                                          const QString &registerContent,
                                          unsigned int currentDataFormatIndex);

  /**
   * \brief dataFormatListForRegister Returns the list of available data
   * format names for the register with the given index to be available to the
   * user for selection. Refer to Register.qml::dataTypeFormatComboBox::model
   * for why this method exists.
   * \param index The index the list of data formats is requested for.
   * \return The list of data format names as QStrings.
   */
  Q_INVOKABLE QStringList
  dataFormatListForRegister(const QModelIndex &index) const;

  /**
   * \brief contentStringForRegister Fetches the current memory value of the
   * given register from the Core and converts it to a string with the
   * data format currently selected for the given register.
   * For a detailed explanation of the use of this method, refer to the
   * description of `data`.
   * \param registerIdentifierContainer QVariant-value containing the identifier
   * of the register whose content is requested.
   * \param currentDataFormatIndex Index of the data format currently active for
   * the register whose content is requested. Required for performing
   * a correct conversion to QString.
   * \return QString-representation of the register's content with the currently
   * active data format.
   */
  Q_INVOKABLE QVariant contentStringForRegister(
      const QModelIndex &index, unsigned int currentDataFormatIndex);

  /**
   * \brief displayFormatStringForRegister Computes a string used for formatting
   * a QTextField via its inputMask-property fitting the currently
   * active data format and the register's properties (e.g. "HH HH HH HH" for
   * 32-bit register with Hex-format).
   * For a detailed explanation of the use of this method, refer to the
   * description of `data`.
   * \param registerIdentifierContainer QVariant-value containing the identifier
   * of the register whose display format string is requested.
   * \param currentDataFormatIndex Index of the data format currently active for
   * the register whose display format string is requested.
   * \return A QString-value in the correct format to be used with the
   * register's QTextField's inputMask-property.
   */
  Q_INVOKABLE QString displayFormatStringForRegister(
      const QModelIndex &index, unsigned int currentDataFormatIndex) const;


 private:
  /// The dummy top-level item holding references to the visible top-level
  /// registers.
  std::unique_ptr<RegisterInformation> _rootItem;

  /// Map of all registers each identified by a unique register identifier.
  std::map<id_t, std::unique_ptr<RegisterInformation>> _items;

  /// Interface for communicating register data changes to core.
  MemoryAccess _memoryAccess;

  /// Associates each register type with a list of avilable data formats.
  const std::map<RegisterInformation::Type, QStringList> _dataFormatLists = {
      {RegisterInformation::Type::INTEGER,
       QStringList() << "Binary"
                     << "Hexadecimal"
                     << "Decimal (Signed)"
                     << "Decimal (Unsigned)"},
      {RegisterInformation::Type::FLOAT,
       QStringList() << "Binary"
                     << "Hexadecimal"
                     << "Float"},
      {RegisterInformation::Type::FLAG,
       QStringList() << "Flag"
                     << "Binary"},
      {RegisterInformation::Type::VECTOR,
       QStringList() << "Binary"
                     << "Hexadecimal"
                     << "Vector"},
      {RegisterInformation::Type::LINK,
       QStringList() << "Binary"
                     << "Hexadecimal"},
      {RegisterInformation::Type::PROGRAM_COUNTER,
       QStringList() << "Binary"
                     << "Hexadecimal"}};

  /**
   * \brief _dataFormatForRegisterItem Retrieves the data format which is
   * currently selected in a certain view of the register.
   * \param registerItem The RegisterInformation object corresponding to the
   * register whose data format is requested.
   * \param currentDataFormatIndex The index of the data format currently
   * selected in a certain view of the register relative to its dataFormat-list
   * (refer to _dataFormatLists).
   * \return The currently selected data format as a QString-value.
   */
  Optional<QString>
  _dataFormatForRegisterItem(const RegisterInformation &registerItem,
                             unsigned int currentDataFormatIndex) const;

  /**
   * \brief _computeDisplayFormatString Computes a string used for formatting a
   * QTextField via its inputMask-property fitting the given data format.
   * \param dataFormat The currently active data format.
   * \param size The register's size.
   * \param byteSize The size of one byte in bits.
   * \return A QString-value in the correct format to be used with a
   * QTextField's inputMask-property.
   */
  QString _computeDisplayFormatString(const QString &dataFormat,
                                      size_t size,
                                      size_t byteSize) const;

  /**
   * \brief _getRowRelativeToParent Returns the row of a given register relative
   * to its parent register.
   * \param registerItem The register whose row is requested.
   * \return The row of the given register relative to its parent register.
   */
  Optional<int>
  _getRowRelativeToParent(RegisterInformation &registerItem) const;
};

#endif// ERAGPSIM_UI_REGISTERMODEL_HPP
