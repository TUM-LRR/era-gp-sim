/*
* C++ Assembler Interpreter
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ui/pixel-display-painted-item.hpp"

#include <algorithm>
#include <ctime>
#include <iostream>

#include "common/assert.hpp"
#include "ui/output-component.hpp"

PixelDisplayPaintedItem::PixelDisplayPaintedItem(QQuickItem *parent)
: QQuickPaintedItem(parent)
, _options{}
, _pixelDisplayErrorFunction{
      [this](bool resolved, const std::string &errMsg) -> void {
        if (resolved) {
          emit pixelDisplayErrorResolved();
        } else {
          emit pixelDisplayError(QString::fromStdString(errMsg));
        }
      }} {
  _options.colorMode = 0;
  _options.width = 16;
  _options.height = 16;
  _image = _options.makeImage();
}

void PixelDisplayPaintedItem::paint(QPainter *painter) {
  painter->drawImage(painter->window(), *_image);
}

void PixelDisplayPaintedItem::memoryChanged(std::size_t address,
                                            std::size_t amount) {
  if (amount == 0 && _outputComponentPointer) {
    amount =
        (*_outputComponentPointer)->getMemoryAccess().getMemorySize().get();
  }
  _options.updateMemory(_outputComponentPointer, _image, address, amount);
  doUpdate();
}

void PixelDisplayPaintedItem::setPixelBaseAddress(size_t pixelBaseAddress) {
  if (_options.pixelBaseAddress != pixelBaseAddress) {
    _options.pixelBaseAddress = pixelBaseAddress;
    _options.updateAllPixels(_outputComponentPointer, _image);
    doUpdate();
  }
}
void PixelDisplayPaintedItem::setColorBaseAddress(size_t colorBaseAddress) {
  if (_options.colorBaseAddress != colorBaseAddress) {
    _options.colorBaseAddress = colorBaseAddress;
    _options.updateAllColors(_outputComponentPointer, _image);
    doUpdate();
  }
}
void PixelDisplayPaintedItem::setWidth(size_t width) {
  if (_options.width != width) {
    resize(width, _options.height);
  }
}
void PixelDisplayPaintedItem::setHeight(size_t height) {
  if (_options.height != height) {
    resize(_options.width, height);
  }
}
void PixelDisplayPaintedItem::setColorMode(const QString &colorMode) {
  size_t colorModeIndex = stringToColorMode(colorMode);
  if (_options.colorMode != colorModeIndex) {
    _options.colorMode = colorModeIndex;
    _image = _options.makeImage();
    _options.updateAllPixels(_outputComponentPointer, _image);
    _options.updateAllColors(_outputComponentPointer, _image);
    doUpdate();
  }
}
void PixelDisplayPaintedItem::setRBit(size_t rBit) {
  if (_options.rBit != rBit) {
    _options.rBit = rBit;
    _options.updateAllPixels(_outputComponentPointer, _image);
    doUpdate();
  }
}
void PixelDisplayPaintedItem::setGBit(size_t gBit) {
  if (_options.gBit != gBit) {
    _options.gBit = gBit;
    _options.updateAllPixels(_outputComponentPointer, _image);
    doUpdate();
  }
}
void PixelDisplayPaintedItem::setBBit(size_t bBit) {
  if (_options.bBit != bBit) {
    _options.bBit = bBit;
    _options.updateAllPixels(_outputComponentPointer, _image);
    doUpdate();
  }
}
void PixelDisplayPaintedItem::setColumns_rows(bool columns_rows) {
  if (_options.columns_rows != columns_rows) {
    _options.columns_rows = columns_rows;
    _options.updateAllPixels(_outputComponentPointer, _image);
    doUpdate();
  }
}
void PixelDisplayPaintedItem::setHorizontallyMirrored(
    bool horizontallyMirrored) {
  // TODO::set flag in painter
}
void PixelDisplayPaintedItem::setVerticallyMirrored(bool verticallyMirrored) {
  // TODO::set flag in painter
}
void PixelDisplayPaintedItem::setTight(bool tight) {
  if (_options.tight != tight) {
    _options.tight = tight;
    _options.updateAllPixels(_outputComponentPointer, _image);
    doUpdate();
  }
}
void PixelDisplayPaintedItem::setPixelBufferPointerLike(
    bool pixelBufferPointerLike) {
  if (_options.pixelBufferPointerLike != pixelBufferPointerLike) {
    _options.pixelBufferPointerLike = pixelBufferPointerLike;
    _options.updateAllPixels(_outputComponentPointer, _image);
    doUpdate();
  }
}
void PixelDisplayPaintedItem::setColorTablePointerLike(
    bool colorTablePointerLike) {
  if (_options.colorTablePointerLike != colorTablePointerLike) {
    _options.colorTablePointerLike = colorTablePointerLike;
    _options.updateAllColors(_outputComponentPointer, _image);
    doUpdate();
  }
}
void PixelDisplayPaintedItem::setFreeBytes(size_t freeBytes) {
  if (_options.freeBytes != freeBytes) {
    _options.freeBytes = freeBytes;
    _options.updateAllPixels(_outputComponentPointer, _image);
    doUpdate();
  }
}
void PixelDisplayPaintedItem::setFreeBits(size_t freeBits) {
  if (_options.freeBits != freeBits) {
    _options.freeBits = freeBits;
    _options.updateAllPixels(_outputComponentPointer, _image);
    doUpdate();
  }
}

size_t PixelDisplayPaintedItem::getPixelBaseAddress() {
  return _options.pixelBaseAddress;
}
size_t PixelDisplayPaintedItem::getColorBaseAddress() {
  return _options.colorBaseAddress;
}
size_t PixelDisplayPaintedItem::getWidth() {
  return _options.width;
}
size_t PixelDisplayPaintedItem::getHeight() {
  return _options.height;
}
QString PixelDisplayPaintedItem::getColorMode() {
  return colorModeToString(_options.colorMode);
}
size_t PixelDisplayPaintedItem::getRBit() {
  return _options.rBit;
}
size_t PixelDisplayPaintedItem::getGBit() {
  return _options.gBit;
}
size_t PixelDisplayPaintedItem::getBBit() {
  return _options.bBit;
}
bool PixelDisplayPaintedItem::getColumns_rows() {
  return _options.columns_rows;
}
bool PixelDisplayPaintedItem::getHorizontallyMirrored() {
  return _options.horizontallyMirrored;
}
bool PixelDisplayPaintedItem::getVerticallyMirrored() {
  return _options.verticallyMirrored;
}
bool PixelDisplayPaintedItem::getTight() {
  return _options.tight;
}
bool PixelDisplayPaintedItem::getPixelBufferPointerLike() {
  return _options.pixelBufferPointerLike;
}
bool PixelDisplayPaintedItem::getColorTablePointerLike() {
  return _options.colorTablePointerLike;
}
size_t PixelDisplayPaintedItem::getFreeBytes() {
  return _options.freeBytes;
}
size_t PixelDisplayPaintedItem::getFreeBits() {
  return _options.freeBits;
}

void PixelDisplayPaintedItem::resize(size_t width, size_t height) {
  if (_options.width != width || _options.height != height) {
    _options.width = width;
    _options.height = height;
    _image = _options.makeImage();
    _options.updateAllPixels(_outputComponentPointer, _image);
    _options.updateAllColors(_outputComponentPointer, _image);
    doUpdate();
  }
}

void PixelDisplayPaintedItem::setOutputComponent(OutputComponent *o) {
  _outputComponentPointer = o;
}

size_t PixelDisplayPaintedItem::stringToColorMode(const QString &colorMode) {
  const QString colorModeNameList[] = {QString::fromStdString("RGB"),
                                       QString::fromStdString("Monochrome")};
  for (std::size_t i = 0; i < 2; ++i) {
    if (colorMode.compare(colorModeNameList[i]) == 0) {
      return i;
    }
  }
  assert::that(false);
  return 0;  // oh what beautiful error code
}
QString PixelDisplayPaintedItem::colorModeToString(size_t colorMode) {
  assert::that(colorMode >= 0);
  assert::that(colorMode < 2);
  const QString colorModeNameList[] = {QString::fromStdString("RGB"),
                                       QString::fromStdString("Monochrome")};
  return colorModeNameList[colorMode];
}

OutputComponent *PixelDisplayPaintedItem::getOutputComponent() {
  // this shouldn't be called ever
  assert::that(false);
  return nullptr;
}

void PixelDisplayPaintedItem::doUpdate() {
  _options.handleErrors(_pixelDisplayErrorFunction);
  update();
}
