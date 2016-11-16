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
#include <QImage>

//class QImage;

class MemoryManager;

class OutputComponent : public QObject {
  Q_OBJECT

 public slots:
  void updateMemory(std::size_t address, std::size_t length);

 signals:
  void memoryChanged(QVariant address, QVariant length);
  void outputItemSettingsChanged();
  void newImage(QImage image);

 public:
  explicit OutputComponent(MemoryManager &memoryManager,
                           MemoryAccess &memoryAccess,
                           QQmlContext *projectContext,
                           QObject *parent = 0);

  Q_INVOKABLE void putMemoryValue(int address, QList<bool> memoryContentBitVector);

  Q_INVOKABLE QList<bool> getMemoryContent(int address, int length) const;

  Q_INVOKABLE QVariantList getOutputItems() const;
  Q_INVOKABLE void setOutputItemProperty(int outputItemIndex, QString property, QVariant newValue);
  Q_INVOKABLE void addOutputItem(QString outputItemType);

private:
  MemoryAccess _memoryAccess;
  QVariantList _outputItems;
};

#endif// ERAGPSIM_UI_OUTPUTCOMPONENT_HPP
