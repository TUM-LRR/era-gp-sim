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

#ifndef ERAGPSIM_UI_OUTPUTCOMPONENT_HPP
#define ERAGPSIM_UI_OUTPUTCOMPONENT_HPP

#include <QDebug>
#include <QQmlContext>
#include "core/memory-access.hpp"

class MemoryManager;

/**
 * \brief The OutputComponent class Manages the model for all output items (i.e.
 * lighstrip, sevensegment etc.) and provides an easy interface for QML
 * components to access data inside the Core (i.e. memory content).
 */
class OutputComponent : public QObject {
  Q_OBJECT

  using size_t = std::size_t;

 public slots:
  /**
   * \brief updateMemory Slot connected to the signal emitted by the Core every
   * time some value inside the memory changes.
   * \param address The address at which the change occured.
   * \param length The length of the area in memory that changed in bytes.
   */
  void updateMemory(std::size_t address, std::size_t length);

 signals:
  /**
   * \brief memoryChanged Signal that the output component calls to signal its
   * QML output item instances that the memory content has changed.
   * \param address The address at which the change occured.
   * \param length The legnth of the area in memory that changed in bytes.
   */
  void memoryChanged(QVariant address, QVariant length);

  /**
   * \brief outputItemSettingsChanged Signal the the output component calls to
   * signal its QML output item instances that the settings of some output
   * item have changed.
   */
  void outputItemSettingsChanged();

 public:
  OutputComponent(MemoryManager &memoryManager,
                  MemoryAccess &memoryAccess,
                  QQmlContext *projectContext,
                  QObject *parent = 0);

  /**
   \brief putMemoryValue Allows a QML output item instance to change the
   memory's
   content at a given address.
   \param address The address at which to put the updated content.
   \param memoryContentBitList The new content in bits.
   */
  Q_INVOKABLE void
  putMemoryValue(int address, QList<bool> memoryContentBitList);

  /**
   \brief getMemoryContent Allows a QML output item instance to fetch the
   memory's
   content at a given address.
   \param address The address at which the requested data lies in memory.
   \param length The length of the requested area in memory.
   \return Bit-Array with the requested memory content.
   */
  Q_INVOKABLE QList<bool> getMemoryContent(int address, int length) const;

  /**
   \brief getOutputItem Returns the model data of the output item with the given
   index.
   \param index The index of the output item whose data is requested.
   \return A map of QVariant-objects containing the output item's output data.
   */
  Q_INVOKABLE QVariant getOutputItem(int index) const;

  /**
   \brief setOutputItemProperty Changes a given property of the model data of a
   given output item.
   \param outputItemIndex The index of the output item whose property should be
   altered.
   \param property Identifier of the property to change.
   \param newValue New value of the given property.
   */
  Q_INVOKABLE void setOutputItemProperty(int outputItemIndex,
                                         QString property,
                                         QVariant newValue);

  /**
    \brief returns the current memory size
    \return The size
    */
  Q_INVOKABLE int getMemorySize();

 private:
  /// Interface for accessing memory content.
  MemoryAccess _memoryAccess;

  /// The underlying model of each output item. Mostly used for savings output
  /// item settings (e.g. baseAddress in memory) as actual data is fetched
  /// dynamically.
  QVariantList _outputItemsInformation;
};

#endif  // ERAGPSIM_UI_OUTPUTCOMPONENT_HPP
