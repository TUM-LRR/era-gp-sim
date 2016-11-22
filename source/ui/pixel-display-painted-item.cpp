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

#include <algorithm>
#include <iostream>

#include "core/conversions.hpp"
#include "core/memory-value.hpp"
#include "ui/pixel-display-painted-item.hpp"

void PixelDisplayPaintedItem::memoryChanged(std::size_t address,
                                            std::size_t amount) {
  for (auto i : pixelsInRange(address, amount)) {
    redrawPixel(i.first, i.second);
  }
}

void PixelDisplayPaintedItem::redrawPixel(std::size_t x, std::size_t y) {
  _image->setPixel(
      x,
      y,
      getColorAt(getBaseAddress() + (x + y * getWidth()) * getCellSize()));
}

void PixelDisplayPaintedItem::redrawAll() {
  for (std::size_t y = 0; y < getHeight(); ++y) {
    for (std::size_t x = 0; x < getWidth(); ++x) {
      redrawPixel(x, y);
    }
  }
}

bool PixelDisplayPaintedItem::isInRange(std::size_t address,
                                        std::size_t amount) const {
  return address >= getBaseAddress() &&
         address + amount < getBaseAddress() + getSizeInMemory();
}

std::vector<std::pair<std::size_t, std::size_t>>
PixelDisplayPaintedItem::pixelsInRange(std::size_t address,
                                       std::size_t amount) const {
  std::vector<std::pair<std::size_t, std::size_t>> ret{};
  for (std::size_t i =
           std::max(std::size_t{0}, address - getBaseAddress()) / getCellSize();
       i < std::min(getSizeInMemory(), address + amount - getBaseAddress()) /
               getCellSize();
       ++i) {
    ret.push_back(std::make_pair(i % getWidth(), i / getWidth()));
  }
  return ret;
}

std::size_t PixelDisplayPaintedItem::getBaseAddress() const {
  return _options.pixelBaseAddress;
}
std::size_t PixelDisplayPaintedItem::getColorMode() const {
  return _options.colorMode;
}
std::size_t PixelDisplayPaintedItem::getWidth() const {
  return _options.width;
}
std::size_t PixelDisplayPaintedItem::getHeight() const {
  return _options.height;
}
std::size_t PixelDisplayPaintedItem::getSizeInMemory() const {
  return getWidth() * getHeight() * getCellSize();
}
std::size_t PixelDisplayPaintedItem::getCellSize() const {
  return 4;
}

std::uint32_t PixelDisplayPaintedItem::getColorAt(std::size_t address) {
  MemoryValue mem{8 * getCellSize()};// just need to get 32 bit of memory here
  return conversions::convert<std::uint32_t>(
      mem, conversions::standardConversions::nonsigned);
}

namespace colorMode {

ColorMode Options::getColorMode() const {
  switch (colorMode) {
    case 0: return RGB;
    default: return RGB;
  }
  return RGB;
}

std::uint32_t Options::getPixel(std::size_t x, std::size_t y) const {
  return getColorMode().getPixel(*this, x, y);
}
void Options::updatePixel(std::shared_ptr<QImage> image,
                          std::size_t x,
                          std::size_t y) const {
  image->setPixel(x, y, getPixel(x, y));
}
void Options::updateMemory(std::shared_ptr<QImage> image,
                           std::size_t address,
                           std::size_t amount) const {
  // TODO
}
void Options::updateAllPixels(std::shared_ptr<QImage> image) const {
  getColorMode().updateAllPixels(*this, image);
}
void Options::updateAllColors(std::shared_ptr<QImage> image) const {
  // TODO
}

const ColorMode::GetPixelFunction ColorMode::RGBGetPixel = [](
    const Options &o, std::size_t x, std::size_t y) -> std::uint32_t {
  std::size_t rBit = 8;    // TODO
  std::size_t gBit = 8;    // TODO
  std::size_t bBit = 8;    // TODO
  std::size_t cellSize = 8;// TODO
  std::size_t sizeInBit = rBit + gBit + bBit;
  std::size_t byteSize =
      (sizeInBit + cellSize - 1) / cellSize + (o.tight ? 0 : o.freeBytes);
  std::size_t index =
      (o.columns_rows ? o.height : 1) + y * (o.columns_rows ? 1 : o.width);
  std::size_t address;
  std::size_t bitOffset = 0;
  if (o.tight) {
    address = o.pixelBaseAddress + (sizeInBit * index) / cellSize;
    bitOffset = (sizeInBit * index) % cellSize;
  } else {
    address = o.pixelBaseAddress + (byteSize)*index;
  }
  MemoryValue mem{32};// just need to get 32 bit of memory here
  if (bitOffset > 0) {// getMemoty(address,byteSize+(bitoffset>0?1:0));
    // TODO::getMemory(address,byteSize+1);
    mem = mem.subSet(bitOffset, bitOffset + sizeInBit);
  } else {
    // TODO::getMemory(address,byteSize);
    mem = mem.subSet(0, sizeInBit);
  }
  MemoryValue blueByte = mem.subSet(0, bBit);
  MemoryValue greenByte = mem.subSet(bBit, bBit + gBit);
  MemoryValue redByte = mem.subSet(bBit + gBit, bBit + gBit + rBit);
  std::uint8_t alpha = 0xFF;
  std::uint8_t red = conversions::convert<std::uint8_t>(
      redByte, conversions::standardConversions::nonsigned);
  std::uint8_t green = conversions::convert<std::uint8_t>(
      greenByte, conversions::standardConversions::nonsigned);
  std::uint8_t blue = conversions::convert<std::uint8_t>(
      blueByte, conversions::standardConversions::nonsigned);
  red <<= 8 - rBit;
  green <<= 8 - gBit;
  blue <<= 8 - bBit;
  return (alpha << 24) | (red << 16) | (green << 8) | (blue);
};

const ColorMode::UpdateMemoryFunction ColorMode::RGBUpdateMemory = [](
    const Options &o,
    std::shared_ptr<QImage> image,
    std::size_t address,
    std::size_t amount) -> void {};

const ColorMode::UpdateAllPixelsFunction ColorMode::RGBUpdateAllPixels = [](
    const Options &o, std::shared_ptr<QImage> image) -> void {
  for (std::size_t y = 0; y < o.height; ++y) {
    for (std::size_t x = 0; x < o.width; ++x) {
      o.updatePixel(image, x, y);
    }
  }
};

const ColorMode::UpdateAllColorsFunction ColorMode::RGBUpdateAllColors = [](
    const Options &o, std::shared_ptr<QImage> image) -> void {};

ColorMode Options::RGB{ColorMode::RGBGetPixel,
                       ColorMode::RGBUpdateMemory,
                       ColorMode::RGBUpdateAllPixels,
                       ColorMode::RGBUpdateAllColors};
}
