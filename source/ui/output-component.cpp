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

#include "core/conversions.hpp"
#include "core/memory-manager.hpp"
#include "core/memory-value.hpp"
#include "ui/output-component.hpp"

OutputComponent::OutputComponent(MemoryManager &memoryManager,
                                 MemoryAccess &memoryAccess,
                                 QQmlContext *projectContext,
                                 QObject *parent)
: _memoryAccess(memoryAccess) {
  // Make this component available to all QML output items.
  projectContext->setContextProperty("outputComponent", this);

  // Add all the output items and their default values.
  _outputItemsInformation.push_back(
      QVariant(QMap<QString, QVariant>{{"type", "LightStrip"},
                                       {"baseAddress", QVariant(0)},
                                       {"numberOfStrips", QVariant(8)}}));
  _outputItemsInformation.push_back(
      QVariant(QMap<QString, QVariant>{{"type", "SevenSegment"},
                                       {"baseAddress", QVariant(0)},
                                       {"numberOfDigits", QVariant(2)}}));
  _outputItemsInformation.push_back(
      QVariant(QMap<QString, QVariant>{{"type", "TextConsole"},
                                       {"baseAddress", QVariant(0)},
                                       {"textMode", QVariant(0)}}));
  _outputItemsInformation.push_back(
      QVariant(QMap<QString, QVariant>{{"type", "PixelDisplay"}));
}


QVariant OutputComponent::getOutputItem(int index) const {
    return _outputItemsInformation[index];
}


<<<<<<< HEAD
void OutputComponent::addOutputItem(QString outputItemType) {
    if (outputItemType == "LightStrip") {
      _outputItems.push_back(
          QVariant(QMap<QString, QVariant>{{"type", "LightStrip"},
                                           {"baseAddress", QVariant(0)},
                                           {"numberOfStrips", QVariant(8)}}));
    } else if (outputItemType == "SevenSegment") {
      _outputItems.push_back(
          QVariant(QMap<QString, QVariant>{{"type", "SevenSegment"},
                                           {"baseAddress", QVariant(0)},
                                           {"numberOfDigits", QVariant(2)}}));
    } else if (outputItemType == "TextConsole") {
      _outputItems.push_back(
          QVariant(QMap<QString, QVariant>{{"type", "TextConsole"},
                                           {"baseAddress", QVariant(0)},
                                           {"textMode", QVariant(0)}}));
    } else if (outputItemType == "PixelDisplay") {
      _outputItems.push_back(
          QVariant(QMap<QString, QVariant>{{"type", "PixelDisplay"},
                                           {"baseAddress", QVariant(0)},
                                           {"breadth", QVariant(320)},
                                           {"height", QVariant(240)},
                                           {"colorMode", QVariant(0)}}));
    }
    == == == = void OutputComponent::setOutputItemProperty(
                 int ouputItemIndex, QString property, QVariant newValue) {
      // Write the new value into the model.
      auto ouputItemProperties =
          _outputItemsInformation[ouputItemIndex].toMap();
      ouputItemProperties[property] = newValue;
      _outputItemsInformation[ouputItemIndex] = ouputItemProperties;
      // Notify all instances of all output items that their settings might have
      // changed.
      emit outputItemSettingsChanged();
>>>>>>> master
    }


    void OutputComponent::updateMemory(size_t address, size_t length) {
      emit memoryChanged(QVariant::fromValue(address),
                         QVariant::fromValue(length));
    }

    void OutputComponent::putMemoryValue(int address,
                                         QList<bool> memoryContentBitList) {
      // Round up size of altered content up to bytes.
      auto memoryValueSize =
          Utility::discreteCeiling(memoryContentBitList.size(), 8);

      // Create new MemoryValue.
      MemoryValue memoryContent(memoryValueSize);
      // Insert the bit values of the altered content into the newly created
      // MemoryValue.
      for (size_t index = 0; index < memoryContentBitList.size(); ++index) {
        memoryContent.put(index, memoryContentBitList.at(index));
      }
      // Pass new value to Core.
      _memoryAccess.putMemoryValueAt(address, memoryContent);
    }

<<<<<<< HEAD
    void OutputComponent::putMemoryValue(int address,
                                         QList<bool> memoryContentBitVector) {
      size_t memoryValueSize = memoryContentBitVector.size() +
                               (8 - (memoryContentBitVector.size() % 8)) % 8;
      MemoryValue memoryContent(memoryValueSize);
      for (size_t index = 0; index < memoryContentBitVector.size(); ++index) {
        memoryContent.put(index, memoryContentBitVector.at(index));
      }
      _memoryAccess.putMemoryValueAt(address, memoryContent);
    }

    QList<bool> OutputComponent::getMemoryContent(int address, int length)
        const {
      MemoryValue content =
          _memoryAccess.getMemoryValueAt(address, length).get();
      QList<bool> contentVector;
      for (auto it = content.cbegin(); it != content.cend(); ++it) {
        contentVector.append(*it);
        == == == = QList<bool> OutputComponent::getMemoryContent(
                     int address, int length) const {
          // Fetch memory content from Core.
          MemoryValue content =
              _memoryAccess.getMemoryValueAt(address, length).get();
          // Convert memory content to a Bit-QList.
          QList<bool> contentList;
          for (const auto &byte : content) {
            contentList.append(byte);
          }
          return contentList;
        }

        QString OutputComponent::getTextFromMemory(
            int start, QString currentText, int mode) {
          std::string text = "";
          if (mode == 0 /*ArrayBased*/) {
            for (int i = 0; (start + i) < _memoryAccess.getMemorySize().get();
                 i++) {
              MemoryValue memoryValue =
                  _memoryAccess.getMemoryValueAt(start + i).get();
              unsigned int z = conversions::convert<uint32_t>(memoryValue);

              if (z == 0) {
                break;
              }

              text += char(z);
            }
          } else /*pipeline*/ {
            text = currentText.toStdString();
            MemoryValue memoryValue =
                _memoryAccess.getMemoryValueAt(start).get();
            unsigned int z = conversions::convert<uint32_t>(memoryValue);
            if (z == 0) {
              text = "";
            } else if (z == 127) {// Delete sign
              text = "";
            } else {
              text += char(z);
>>>>>>> master
            }
          }
          return QString::fromStdString(text);
        }

        int OutputComponent::getMemorySize() {
          return _memoryAccess.getMemorySize().get();
        }
