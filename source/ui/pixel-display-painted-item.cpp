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
    case 1: return Monochrome;
    default: return RGB;
  }
  return RGB;
}

std::uint32_t Options::getPixel(Optional<OutputComponent *> memoryAccess,
                                std::size_t x,
                                std::size_t y) const {
  return getColorMode().getPixel(memoryAccess, *this, x, y);
}
std::uint32_t Options::getColor(Optional<OutputComponent *> memoryAccess,
                                std::size_t index) const {
  return getColorMode().getColor(memoryAccess, *this, index);
}
void Options::updatePixel(Optional<OutputComponent *> memoryAccess,
                          std::shared_ptr<QImage> image,
                          std::size_t x,
                          std::size_t y) const {
  image->setPixel(x, y, getPixel(memoryAccess, x, y));
}
void Options::updateColor(Optional<OutputComponent *> memoryAccess,
                          std::shared_ptr<QImage> image,
                          std::size_t index) const {
  image->setColor(index, getColor(memoryAccess, index));
}
void Options::updateMemory(Optional<OutputComponent *> memoryAccess,
                           std::shared_ptr<QImage> image,
                           std::size_t address,
                           std::size_t amount) const {
  getColorMode().updateMemory(memoryAccess, *this, image, address, amount);
}
void Options::updateAllPixels(Optional<OutputComponent *> memoryAccess,
                              std::shared_ptr<QImage> image) const {
  if (memoryAccess)
    std::cout << "I have fuckin memory access, yeah!" << std::endl;
  getColorMode().updateAllPixels(memoryAccess, *this, image);
}
void Options::updateAllColors(Optional<OutputComponent *> memoryAccess,
                              std::shared_ptr<QImage> image) const {
  getColorMode().updateAllColors(memoryAccess, *this, image);
}

// RGB*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*
const ColorMode::GetPixelFunction ColorMode::RGBGetPixel = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::size_t x,
    std::size_t y) -> std::uint32_t {
  std::size_t cellSize = 8;// TODO
  std::size_t sizeInBit = o.rBit + o.gBit + o.bBit;
  std::size_t byteSize =
      (sizeInBit + cellSize - 1) / cellSize + (o.tight ? 0 : o.freeBytes);
  std::size_t index =
      x * (o.columns_rows ? o.height : 1) + y * (o.columns_rows ? 1 : o.width);
  std::size_t address;
  std::size_t bitOffset = 0;
  if (o.tight) {
    address = o.pixelBaseAddress + (sizeInBit * index) / cellSize;
    bitOffset = (sizeInBit * index) % cellSize;
  } else {
    address = o.pixelBaseAddress + (byteSize)*index;
  }
  // TODO::*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*
  // the memory access goes somewhere here, kinda
  MemoryValue mem{32};// just need to get 32 bit of memory here
  if (bitOffset > 0) {// getMemoty(address,byteSize+(bitoffset>0?1:0));
    // TODO::getMemory(address,byteSize+1);
    mem = mem.subSet(bitOffset, bitOffset + sizeInBit);
  } else {
    // TODO::getMemory(address,byteSize);
    mem = mem.subSet(0, sizeInBit);
  }
  // *-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*
  MemoryValue blueByte = mem.subSet(0, o.bBit);
  MemoryValue greenByte = mem.subSet(o.bBit, o.bBit + o.gBit);
  MemoryValue redByte = mem.subSet(o.bBit + o.gBit, o.bBit + o.gBit + o.rBit);
  std::uint8_t alpha = 0xFF;
  std::uint8_t red = conversions::convert<std::uint8_t>(
      redByte, conversions::standardConversions::nonsigned);
  std::uint8_t green = conversions::convert<std::uint8_t>(
      greenByte, conversions::standardConversions::nonsigned);
  std::uint8_t blue = conversions::convert<std::uint8_t>(
      blueByte, conversions::standardConversions::nonsigned);
  red = ((4 * x / 256 % 3 == 0) ? 0 : (4 * x)) % 256;
  green = ((4 * x / 256 % 3 == 1) ? 0 : (4 * x)) % 256;
  blue = ((4 * x / 256 % 3 == 2) ? 0 : (4 * x)) % 256;
  red <<= 8 - o.rBit;
  green <<= 8 - o.gBit;
  blue <<= 8 - o.bBit;
  return (alpha << 24) | (red << 16) | (green << 8) | (blue);
};

const ColorMode::GetColorFunction ColorMode::RGBGetColor = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::size_t index) -> std::uint32_t { return 0; };

const ColorMode::UpdateMemoryFunction ColorMode::RGBUpdateMemory = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::shared_ptr<QImage> image,
    std::size_t address,
    std::size_t amount) -> void {
  std::size_t cellSize = 8;// TODO
  std::size_t sizeInBit = o.rBit + o.gBit + o.bBit;
  std::size_t byteSize =
      (sizeInBit + cellSize - 1) / cellSize + (o.tight ? 0 : o.freeBytes);
  std::size_t pixelSize =
      (o.tight ? ((sizeInBit * o.width * o.height + cellSize - 1) / cellSize)
               : (((sizeInBit + cellSize - 1) / cellSize + o.freeBytes) *
                  o.height * o.width));
  std::size_t colorSize = 0;
  if (address < o.pixelBaseAddress + pixelSize &&
      address + amount > o.pixelBaseAddress) {
    // Pixel Memory has been updated => update pixels in image
    std::size_t minAddress = std::max(address, o.pixelBaseAddress);
    std::size_t maxAddress =
        std::min(address + amount, o.pixelBaseAddress + pixelSize);
    std::size_t minIndex =
        o.tight ? ((minAddress - o.pixelBaseAddress) * cellSize / sizeInBit)
                : ((minAddress - o.pixelBaseAddress) / byteSize);
    std::size_t maxIndex =
        o.tight
            ? (((maxAddress - o.pixelBaseAddress) * cellSize + cellSize - 1) /
               sizeInBit)
            : ((maxAddress - o.pixelBaseAddress + byteSize - 1) / byteSize);
    // TODO::make this more efficient evtl
    for (std::size_t i = minIndex; i <= maxIndex; ++i) {
      o.updatePixel(memoryAccess,
                    image,
                    o.columns_rows ? (i / o.height) : (i % o.width),
                    o.columns_rows ? (i % o.height) : (i / o.width));
    }
  }
  // if (address < o.colorBaseAddress + colorSize &&
  //     address + amount > o.colorBaseAddress) {
  //   // Color Memory has been updated => update colors in image
  // }

};

const ColorMode::UpdateAllPixelsFunction ColorMode::RGBUpdateAllPixels = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::shared_ptr<QImage> image) -> void {
  for (std::size_t y = 0; y < o.height; ++y) {
    for (std::size_t x = 0; x < o.width; ++x) {
      o.updatePixel(memoryAccess, image, x, y);
    }
  }
};

const ColorMode::UpdateAllColorsFunction ColorMode::RGBUpdateAllColors = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::shared_ptr<QImage> image) -> void {};

// Monochrome*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*
const ColorMode::GetPixelFunction ColorMode::MonochromeGetPixel = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::size_t x,
    std::size_t y) -> std::uint32_t {
  std::size_t cellSize = 8;          // TODO
  std::size_t bitsPerByte = cellSize;// TODO
  std::size_t index =
      x * (o.columns_rows ? o.height : 1) + y * (o.columns_rows ? 1 : o.width);
  std::size_t address = o.pixelBaseAddress + index / bitsPerByte;
  std::size_t bitOffset = index % bitsPerByte;
  // TODO::*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*
  // the memory access goes somewhere here, kinda
  MemoryValue mem;
  if (memoryAccess) {
    auto m = ((*memoryAccess)->getMemoryAccess().getMemoryValueAt(address, 1));
    mem = m.get();
  } else {
    mem = MemoryValue(cellSize);
  }
  // just need to get some bit of memory here
  bool b = mem.get(bitOffset);
  // *-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*
  //b = (x + y) % 2 == 0;
  return b ? 1 : 0;
};
const ColorMode::GetColorFunction ColorMode::MonochromeGetColor = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::size_t index) -> std::uint32_t {
  std::size_t cellSize = 8;// TODO
  std::size_t colorSize = (32 + cellSize - 1) / cellSize;
  std::size_t colorTablePointer = o.colorBaseAddress;
  if (o.colorTablePointerLike) {
    MemoryValue colorTableBaseAddress{32};// TODO::get pointer size
    // TODO::memory.get(o.colorBaseAddress,colorSize);
    colorTablePointer = conversions::convert<std::size_t>(
        colorTableBaseAddress, conversions::standardConversions::nonsigned);
  }
  MemoryValue color{32};
  // TODO::memory.get(colorTablePointer+colorSize*index,colorSize);
  return 0xFFFFFFFFu * index;
  // return conversions::convert<std::uint32_t>(
  //    color, conversions::standardConversions::nonsigned);
  ;
};
const ColorMode::UpdateMemoryFunction ColorMode::MonochromeUpdateMemory = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::shared_ptr<QImage> image,
    std::size_t address,
    std::size_t amount) -> void {
  std::size_t cellSize = 8;          // TODO
  std::size_t bitsPerByte = cellSize;// TODO
  std::size_t colorCount = 2;
  std::size_t size32 = (32 + cellSize - 1) / cellSize;
  std::size_t colorSize = size32 * colorCount;
  std::size_t pixelSize = o.height * o.width / bitsPerByte;
  if (address < o.pixelBaseAddress + pixelSize &&
      address + amount > o.pixelBaseAddress) {
    // Pixel Memory has been updated => update pixels in image
    std::size_t minAddress = std::max(address, o.pixelBaseAddress);
    std::size_t maxAddress =
        std::min(address + amount, o.pixelBaseAddress + pixelSize);
    std::size_t minIndex = (minAddress - o.pixelBaseAddress) * bitsPerByte;
    std::size_t maxIndex =
        (maxAddress - o.pixelBaseAddress) * bitsPerByte + bitsPerByte - 1;
    // TODO::make this more efficient evtl
    for (std::size_t i = minIndex; i <= maxIndex; ++i) {
      o.updatePixel(memoryAccess,
                    image,
                    o.columns_rows ? (i / o.height) : (i % o.width),
                    o.columns_rows ? (i % o.height) : (i / o.width));
    }
  }
  if (o.colorTablePointerLike && address < o.colorBaseAddress + size32 &&
      address + amount > o.colorBaseAddress) {
    // the color table pointer has been changed => update all colors
    // TODO
  } else {
    std::size_t colorTablePointer = o.colorBaseAddress;
    if (o.colorTablePointerLike) {
      MemoryValue colorTableBaseAddress{32};// TODO::get pointer size
      // TODO::memory.get(o.colorBaseAddress,colorSize);
      colorTablePointer = conversions::convert<std::size_t>(
          colorTableBaseAddress, conversions::standardConversions::nonsigned);
    }
    if (address < colorTablePointer + colorSize &&
        address + amount > colorTablePointer) {
      // Color Memory has been updated => update colors in image
      std::size_t minAddress = std::max(address, colorTablePointer);
      std::size_t maxAddress =
          std::min(address + amount, colorTablePointer + colorSize);
      std::size_t minIndex = (minAddress - colorTablePointer) / size32;
      std::size_t maxIndex =
          (maxAddress - colorTablePointer + size32 - 1) / size32;
      for (std::size_t i = minIndex; i <= maxIndex; ++i) {
        o.updateColor(memoryAccess, image, i);
      }
    }
  }
};
const ColorMode::UpdateAllPixelsFunction ColorMode::MonochromeUpdateAllPixels =
    [](Optional<OutputComponent *> memoryAccess,
       const Options &o,
       std::shared_ptr<QImage> image) -> void {
  for (std::size_t y = 0; y < o.height; ++y) {
    for (std::size_t x = 0; x < o.width; ++x) {
      o.updatePixel(memoryAccess, image, x, y);
    }
  }
};
const ColorMode::UpdateAllColorsFunction ColorMode::MonochromeUpdateAllColors =
    [](Optional<OutputComponent *> memoryAccess,
       const Options &o,
       std::shared_ptr<QImage> image) -> void {
  std::size_t colorCount = 2;
  for (std::size_t i = 0; i < colorCount; ++i) {
    o.updateColor(memoryAccess, image, i);
  }
};

// ColorMode*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*
ColorMode Options::RGB{ColorMode::RGBGetPixel,
                       ColorMode::RGBGetColor,
                       ColorMode::RGBUpdateMemory,
                       ColorMode::RGBUpdateAllPixels,
                       ColorMode::RGBUpdateAllColors};
ColorMode Options::Monochrome{ColorMode::MonochromeGetPixel,
                              ColorMode::MonochromeGetColor,
                              ColorMode::MonochromeUpdateMemory,
                              ColorMode::MonochromeUpdateAllPixels,
                              ColorMode::MonochromeUpdateAllColors};
}
