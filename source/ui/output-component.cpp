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

#include "ui/output-component.hpp"
#include "core/memory-manager.hpp"
#include "core/memory-value.hpp"
#include "core/conversions.hpp"
#include <QQuickView>
#include <QQuickItem>

OutputComponent::OutputComponent(MemoryManager &memoryManager,
                                 MemoryAccess &memoryAccess,
                                 QQmlContext *projectContext,
                                 QObject *parent)
    : _memoryAccess(memoryAccess) {
    projectContext->setContextProperty("outputComponent", this);

    _outputItems.push_back(QVariant(QMap<QString, QVariant>{{"type", "LightStrip"}, {"baseAddress", QVariant(0)}, {"numberOfStrips", QVariant(8)}}));
    _outputItems.push_back(QVariant(QMap<QString, QVariant>{{"type", "SevenSegment"}, {"baseAddress", QVariant(0)}, {"numberOfDigits", QVariant(2)}}));
    _outputItems.push_back(QVariant(QMap<QString, QVariant>{{"type", "TextConsole"}, {"baseAddress", QVariant(0)}, {"textMode", QVariant(0)}}));
    _outputItems.push_back(QVariant(QMap<QString, QVariant>{{"type", "PixelDisplay"}, {"baseAddress", QVariant(0)}}));
  }


QVariantList OutputComponent::getOutputItems() const {
    return _outputItems;
}


void OutputComponent::addOutputItem(QString outputItemType) {
    if (outputItemType == "LightStrip") {
        _outputItems.push_back(QVariant(QMap<QString, QVariant>{{"type", "LightStrip"}, {"baseAddress", QVariant(0)}, {"numberOfStrips", QVariant(8)}}));
    } else if (outputItemType == "SevenSegment") {
        _outputItems.push_back(QVariant(QMap<QString, QVariant>{{"type", "SevenSegment"}, {"baseAddress", QVariant(0)}, {"numberOfDigits", QVariant(2)}}));
    } else if (outputItemType == "TextConsole") {
        _outputItems.push_back(QVariant(QMap<QString, QVariant>{{"type", "TextConsole"}, {"baseAddress", QVariant(0)}, {"textMode", QVariant(0)}}));
    } else if (outputItemType == "PixelDisplay") {
      _outputItems.push_back(QVariant(QMap<QString, QVariant>{{"type", "PixelDisplay"}, {"baseAddress", QVariant(0)}, {"breadth", QVariant(320)}, {"height", QVariant(240)}, {"colorMode", QVariant(0)}}));
    }
}

void OutputComponent::setOutputItemProperty(int ouputItemIndex, QString property, QVariant newValue) {
    QMap<QString, QVariant> ouputItemProperties = _outputItems[ouputItemIndex].toMap();
    ouputItemProperties[property] = newValue;
    _outputItems[ouputItemIndex] = ouputItemProperties;
    emit outputItemSettingsChanged();
}


void OutputComponent::updateMemory(std::size_t address, std::size_t length) {
    emit memoryChanged(QVariant::fromValue(address), QVariant::fromValue(length));
}

void OutputComponent::putMemoryValue(int address, QList<bool> memoryContentBitVector) {
    size_t memoryValueSize = memoryContentBitVector.size() + (8 - (memoryContentBitVector.size()%8))%8;
    MemoryValue memoryContent(memoryValueSize);
    for (size_t index = 0; index < memoryContentBitVector.size(); ++index) {
        memoryContent.put(index, memoryContentBitVector.at(index));
    }
    _memoryAccess.putMemoryValueAt(address, memoryContent);
}

QList<bool> OutputComponent::getMemoryContent(int address, int length) const {
    MemoryValue content = _memoryAccess.getMemoryValueAt(address, length).get();
    QList<bool> contentVector;
    for (auto it = content.cbegin(); it != content.cend(); ++it) {
        contentVector.append(*it);
    }
    return contentVector;
}
