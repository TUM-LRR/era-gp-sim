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

#include "common/assert.hpp"
#include "core/conversions.hpp"
#include "core/memory-value.hpp"
#include "ui/pixel-display-painted-item.hpp"

void PixelDisplayPaintedItem::memoryChanged(std::size_t address,
                                            std::size_t amount) {
  std::cout << "PixelDisplayPaintedItem memory changed" << std::endl;
  //_options.updateAllPixels(_outputComponentPointer, _image);
  //_options.updateAllColors(_outputComponentPointer, _image);
  _options.updateMemory(_outputComponentPointer, _image, address, amount);
  // TODO::use efficient updateMemory
  update();
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
std::uint32_t Options::getPixelFromBuffer(const MemoryValue &buffer,
                                          std::size_t offset,
                                          std::size_t x,
                                          std::size_t y) const {
  return getColorMode().getPixelFromBuffer(buffer, offset, *this, x, y);
}
std::uint32_t Options::getColorFromBuffer(const MemoryValue &buffer,
                                          std::size_t offset,
                                          std::size_t index) const {
  return getColorMode().getColorFromBuffer(buffer, offset, *this, index);
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
  std::size_t cellSize = 8;    // TODO
  std::size_t pointerSize = 32;// TODO
  std::size_t pixelBufferPointer = o.pixelBaseAddress;
  if (o.pixelBufferPointerLike) {
    MemoryValue pixelPointer{pointerSize};
    if (memoryAccess) {
      pixelPointer =
          (*memoryAccess)
              ->getMemoryAccess()
              .getMemoryValueAt(pixelBufferPointer,
                                (pointerSize + cellSize - 1) / cellSize)
              .get();
    }
    pixelBufferPointer = conversions::convert<std::size_t>(
        pixelPointer, conversions::standardConversions::nonsigned);
  }
  std::size_t sizeInBit = o.rBit + o.gBit + o.bBit;
  std::size_t byteSize =
      (sizeInBit + cellSize - 1) / cellSize + (o.tight ? 0 : o.freeBytes);
  std::size_t index =
      x * (o.columns_rows ? o.height : 1) + y * (o.columns_rows ? 1 : o.width);
  std::size_t address = pixelBufferPointer;
  std::size_t bitOffset = 0;
  if (o.tight) {
    address += (sizeInBit * index) / cellSize;
    bitOffset = (sizeInBit * index) % cellSize;
  } else {
    address += byteSize * index;
  }
  MemoryValue buffer{1};
  if (memoryAccess) {
    buffer = (*memoryAccess)
                 ->getMemoryAccess()
                 .getMemoryValueAt(address, byteSize + (bitOffset > 0 ? 1 : 0))
                 .get();
  }
  return o.getPixelFromBuffer(buffer, address, x, y);
};

const ColorMode::GetColorFunction ColorMode::RGBGetColor = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::size_t index) -> std::uint32_t { return 0; };

const ColorMode::GetPixelFromBufferFunction ColorMode::RGBGetPixelFromBuffer =
    [](const MemoryValue &buffer,
       std::size_t offset,
       const Options &o,
       std::size_t x,
       std::size_t y) -> std::uint32_t {
  std::size_t cellSize = 8;// TODO
  std::size_t sizeInBit = o.rBit + o.gBit + o.bBit;
  std::size_t byteSize =
      (sizeInBit + cellSize - 1) / cellSize + (o.tight ? 0 : o.freeBytes);
  std::size_t index =
      x * (o.columns_rows ? o.height : 1) + y * (o.columns_rows ? 1 : o.width);
  std::size_t beginBit = index * (o.tight ? sizeInBit : (byteSize * cellSize));
  std::size_t bitOffset = beginBit - (offset * cellSize);
  // Load from buffer!
  MemoryValue mem{sizeInBit};
  if (buffer.getSize() > bitOffset + sizeInBit) {
    mem = buffer.subSet(bitOffset, bitOffset + sizeInBit);
  } else {
    return 0xFFFF00FF;
  }
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
  red <<= 8 - o.rBit;
  green <<= 8 - o.gBit;
  blue <<= 8 - o.bBit;
  return (alpha << 24) | (red << 16) | (green << 8) | (blue);
};
const ColorMode::GetColorFromBufferFunction ColorMode::RGBGetColorFromBuffer =
    [](const MemoryValue &buffer,
       std::size_t offset,
       const Options &o,
       std::size_t index) -> std::uint32_t { return 0; };

const ColorMode::UpdateMemoryFunction ColorMode::RGBUpdateMemory = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::shared_ptr<QImage> image,
    std::size_t address,
    std::size_t amount) -> void {
  std::size_t cellSize = 8;    // TODO
  std::size_t pointerSize = 32;// TODO
  std::size_t pointerSizeInByte = (pointerSize + cellSize - 1) / cellSize;
  std::size_t pixelBufferPointer = o.pixelBaseAddress;
  bool hasAlreadyUpdatedAllPixels = false;
  if (o.pixelBufferPointerLike) {
    if (address < pixelBufferPointer + pointerSizeInByte &&
        address + amount > pixelBufferPointer) {
      // pixel pointer has changed
      o.updateAllPixels(memoryAccess, image);
      hasAlreadyUpdatedAllPixels = true;
    } else {
      MemoryValue pixelPointer{pointerSize};
      if (memoryAccess) {
        pixelPointer =
            (*memoryAccess)
                ->getMemoryAccess()
                .getMemoryValueAt(pixelBufferPointer,
                                  (pointerSize + cellSize - 1) / cellSize)
                .get();
      }
      pixelBufferPointer = conversions::convert<std::size_t>(
          pixelPointer, conversions::standardConversions::nonsigned);
    }
  }
  std::size_t sizeInBit = o.rBit + o.gBit + o.bBit;
  std::size_t pixelBufferSize =
      (o.tight ? ((sizeInBit * o.width * o.height + cellSize - 1) / cellSize)
               : (((sizeInBit + cellSize - 1) / cellSize + o.freeBytes) *
                  o.height * o.width));
  if (!hasAlreadyUpdatedAllPixels) {
    if (address < pixelBufferPointer + pixelBufferSize &&
        address + amount > pixelBufferPointer) {
      // At least some pixel have changed
      std::size_t sizeInByte =
          (sizeInBit + cellSize - 1) / cellSize + (o.tight ? 0 : o.freeBytes);
      std::size_t minIndex = 0;
      if (address > pixelBufferPointer) {
        minIndex = o.tight
                       ? ((address - pixelBufferPointer) * cellSize / sizeInBit)
                       : ((address - pixelBufferPointer) / sizeInByte);
      }
      std::size_t maxIndex = o.height * o.width;
      if (address + amount < pixelBufferPointer + pixelBufferSize) {
        maxIndex =
            o.tight
                ? (((address + amount - pixelBufferPointer) * cellSize +
                    sizeInBit - 1) /
                   sizeInBit)
                : (((address + amount - pixelBufferPointer) + sizeInByte - 1) /
                   sizeInByte);
      }
      std::size_t beginOffset =
          o.tight ? (minIndex * sizeInBit / cellSize) : (minIndex * sizeInByte);
      std::size_t endOffset =
          o.tight ? (maxIndex * sizeInBit / cellSize) : (maxIndex * sizeInByte);
      MemoryValue buffer{1};
      if (memoryAccess) {
        buffer = (*memoryAccess)
                     ->getMemoryAccess()
                     .getMemoryValueAt(pixelBufferPointer + beginOffset,
                                       endOffset - beginOffset)
                     .get();
      }
      for (std::size_t index = minIndex; index < maxIndex; index++) {
        std::size_t x = o.columns_rows ? (index / o.height) : (index % o.width);
        std::size_t y = o.columns_rows ? (index % o.height) : (index / o.width);
        image->setPixel(x, y, o.getPixelFromBuffer(buffer, beginOffset, x, y));
      }
    }
  }
};

const ColorMode::UpdateAllPixelsFunction ColorMode::RGBUpdateAllPixels = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::shared_ptr<QImage> image) -> void {
  std::size_t cellSize = 8;    // TODO
  std::size_t pointerSize = 32;// TODO
  std::size_t pixelBufferPointer = o.pixelBaseAddress;
  if (o.pixelBufferPointerLike) {
    MemoryValue pixelPointer{pointerSize};
    if (memoryAccess) {
      pixelPointer =
          (*memoryAccess)
              ->getMemoryAccess()
              .getMemoryValueAt(pixelBufferPointer,
                                (pointerSize + cellSize - 1) / cellSize)
              .get();
    }
    pixelBufferPointer = conversions::convert<std::size_t>(
        pixelPointer, conversions::standardConversions::nonsigned);
  }
  std::size_t sizeOfColorInBit = o.rBit + o.gBit + o.bBit;
  if (!o.tight) {
    sizeOfColorInBit =
        ((sizeOfColorInBit + cellSize - 1) / cellSize + o.freeBytes) * cellSize;
  }
  std::size_t sizeInMemory =
      (sizeOfColorInBit * o.width * o.height + cellSize - 1) / cellSize;
  MemoryValue buffer{1};
  if (memoryAccess) {
    buffer = (*memoryAccess)
                 ->getMemoryAccess()
                 .getMemoryValueAt(pixelBufferPointer, sizeInMemory)
                 .get();
  }
  for (std::size_t y = 0; y < o.height; ++y) {
    for (std::size_t x = 0; x < o.width; ++x) {
      image->setPixel(x, y, o.getPixelFromBuffer(buffer, 0, x, y));
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
  MemoryValue buffer{1};
  if (memoryAccess) {
    buffer =
        (*memoryAccess)->getMemoryAccess().getMemoryValueAt(address, 1).get();
  }
  return o.getPixelFromBuffer(buffer, address, x, y);
};
const ColorMode::GetColorFunction ColorMode::MonochromeGetColor = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::size_t index) -> std::uint32_t {
  std::size_t cellSize = 8;    // TODO
  std::size_t pointerSize = 32;// TODO
  constexpr std::size_t colorSizeInBit = 32;
  std::size_t colorSize = (32 + cellSize - 1) / cellSize;
  std::size_t colorTablePointer = o.colorBaseAddress;
  if (o.colorTablePointerLike) {
    MemoryValue colorTableBaseAddress{pointerSize};
    if (memoryAccess) {
      colorTableBaseAddress =
          (*memoryAccess)
              ->getMemoryAccess()
              .getMemoryValueAt(colorTablePointer,
                                (pointerSize + cellSize - 1) / cellSize)
              .get();
    }
    colorTablePointer = conversions::convert<std::size_t>(
        colorTableBaseAddress, conversions::standardConversions::nonsigned);
  }
  MemoryValue buffer{colorSizeInBit};
  if (memoryAccess) {
    buffer = (*memoryAccess)
                 ->getMemoryAccess()
                 .getMemoryValueAt(colorTablePointer + (index * colorSize),
                                   colorSize)
                 .get();
  }
  return o.getColorFromBuffer(buffer, index * colorSize, index);
};

const ColorMode::GetPixelFromBufferFunction
    ColorMode::MonochromeGetPixelFromBuffer =
        [](const MemoryValue &buffer,
           std::size_t offset,
           const Options &o,
           std::size_t x,
           std::size_t y) -> std::uint32_t {
  std::size_t cellSize = 8;// TODO
  std::size_t bitsPerByte = cellSize - o.freeBits;
  std::size_t index =
      x * (o.columns_rows ? o.height : 1) + y * (o.columns_rows ? 1 : o.width);
  std::size_t address =
      (index / bitsPerByte * cellSize) + (index % bitsPerByte);
  if (buffer.getSize() >= address) {
    return buffer.get(address) ? 1 : 0;
  }
  return 0;
};
const ColorMode::GetColorFromBufferFunction
    ColorMode::MonochromeGetColorFromBuffer =
        [](const MemoryValue &buffer,
           std::size_t offset,
           const Options &o,
           std::size_t index) -> std::uint32_t {
  std::size_t cellSize = 8;// TODO
  constexpr std::size_t colorSizeInBit = 32;
  std::size_t colorSize = (colorSizeInBit + cellSize - 1) / cellSize;
  std::size_t byteOffset = (colorSize * index) - offset;
  if (buffer.getSize() >= byteOffset * cellSize + colorSizeInBit) {
    MemoryValue color = buffer.subSet(byteOffset * cellSize,
                                      byteOffset * cellSize + colorSizeInBit);
    return conversions::convert<std::uint32_t>(
        color, conversions::standardConversions::nonsigned);
  }
  return 0xFFFF00FF;// error color
};

const ColorMode::UpdateMemoryFunction ColorMode::MonochromeUpdateMemory = [](
    Optional<OutputComponent *> memoryAccess,
    const Options &o,
    std::shared_ptr<QImage> image,
    std::size_t address,
    std::size_t amount) -> void {
  std::size_t cellSize = 8;    // TODO
  std::size_t pointerSize = 32;// TODO
  std::size_t pointerSizeInByte = (pointerSize + cellSize - 1) / cellSize;
  std::size_t pixelBufferPointer = o.pixelBaseAddress;
  bool hasAlreadyUpdatedAllPixels = false;
  if (o.pixelBufferPointerLike) {
    if (address < pixelBufferPointer + pointerSizeInByte &&
        address + amount > pixelBufferPointer) {
      // pixel pointer has changed
      o.updateAllPixels(memoryAccess, image);
      hasAlreadyUpdatedAllPixels = true;
    } else {
      MemoryValue pixelPointer{pointerSize};
      if (memoryAccess) {
        pixelPointer =
            (*memoryAccess)
                ->getMemoryAccess()
                .getMemoryValueAt(pixelBufferPointer,
                                  (pointerSize + cellSize - 1) / cellSize)
                .get();
      }
      pixelBufferPointer = conversions::convert<std::size_t>(
          pixelPointer, conversions::standardConversions::nonsigned);
    }
  }
  std::size_t pixelBufferSize = (o.width * o.height + cellSize - 1) / cellSize;
  if (!hasAlreadyUpdatedAllPixels) {
    if (address < pixelBufferPointer + pixelBufferSize &&
        address + amount > pixelBufferPointer) {
      // At least some pixel have changed
      std::size_t bitsPerByte = cellSize - o.freeBits;
      std::size_t minIndex = 0;
      if (address > pixelBufferPointer) {
        minIndex = (address - pixelBufferPointer) * bitsPerByte;
      }
      std::size_t maxIndex = o.height * o.width;
      if (address + amount < pixelBufferPointer + pixelBufferSize) {
        maxIndex = (address + amount - pixelBufferPointer) * bitsPerByte;
      }
      std::size_t beginOffset = minIndex / bitsPerByte;
      std::size_t endOffset = (maxIndex + bitsPerByte - 1) / bitsPerByte;
      // TODO from here
      MemoryValue buffer{1};
      if (memoryAccess) {
        buffer = (*memoryAccess)
                     ->getMemoryAccess()
                     .getMemoryValueAt(pixelBufferPointer,
                                       pixelBufferSize)// this could be
                                                       // optimized, we don't
                                                       // actually need the
                                                       // whole pixel buffer,
                                                       // but anyways
                     //  .getMemoryValueAt(pixelBufferPointer + beginOffset,
                     //                    endOffset - beginOffset)
                     .get();
      }
      for (std::size_t index = minIndex; index < maxIndex; index++) {
        std::size_t x = o.columns_rows ? (index / o.height) : (index % o.width);
        std::size_t y = o.columns_rows ? (index % o.height) : (index / o.width);
        image->setPixel(x, y, o.getPixelFromBuffer(buffer, 0, x, y));
      }
    }
  }
  // the colors
  std::size_t colorBufferPointer = o.colorBaseAddress;
  bool hasAlreadyUpdatedAllColors = false;
  if (o.colorTablePointerLike) {
    if (address < colorBufferPointer + pointerSizeInByte &&
        address + amount > colorBufferPointer) {
      // pixel pointer has changed
      o.updateAllColors(memoryAccess, image);
      hasAlreadyUpdatedAllColors = true;
    } else {
      MemoryValue colorPointer{pointerSize};
      if (memoryAccess) {
        colorPointer =
            (*memoryAccess)
                ->getMemoryAccess()
                .getMemoryValueAt(colorBufferPointer,
                                  (pointerSize + cellSize - 1) / cellSize)
                .get();
      }
      colorBufferPointer = conversions::convert<std::size_t>(
          colorPointer, conversions::standardConversions::nonsigned);
    }
  }
  constexpr std::size_t colorCount = 2;
  constexpr std::size_t colorSizeInBit = 32;
  std::size_t colorSizeInByte = (colorSizeInBit + cellSize - 1) / cellSize;
  std::size_t colorBufferSize = colorCount * colorSizeInByte;
  if (!hasAlreadyUpdatedAllColors) {
    if (address < colorBufferPointer + colorBufferSize &&
        address + amount > colorBufferPointer) {
      // At least some colors have changed
      o.updateAllColors(memoryAccess, image);
      // this could be optimized, but 2 colors are not many enough to really
      // need that
    }
  }
};

const ColorMode::UpdateAllPixelsFunction ColorMode::MonochromeUpdateAllPixels =
    [](Optional<OutputComponent *> memoryAccess,
       const Options &o,
       std::shared_ptr<QImage> image) -> void {
  std::size_t cellSize = 8;    // TODO
  std::size_t pointerSize = 32;// TODO
  std::size_t pixelBufferPointer = o.pixelBaseAddress;
  if (o.pixelBufferPointerLike) {
    MemoryValue pixelPointer{pointerSize};
    if (memoryAccess) {
      pixelPointer =
          (*memoryAccess)
              ->getMemoryAccess()
              .getMemoryValueAt(pixelBufferPointer,
                                (pointerSize + cellSize - 1) / cellSize)
              .get();
    }
    pixelBufferPointer = conversions::convert<std::size_t>(
        pixelPointer, conversions::standardConversions::nonsigned);
  }
  std::size_t bitsPerByte = cellSize - o.freeBits;

  std::size_t sizeInMemory =
      (o.width * o.height + bitsPerByte - 1) / bitsPerByte;
  MemoryValue buffer{1};
  if (memoryAccess) {
    buffer = (*memoryAccess)
                 ->getMemoryAccess()
                 .getMemoryValueAt(pixelBufferPointer, sizeInMemory)
                 .get();
  }
  for (std::size_t y = 0; y < o.height; ++y) {
    for (std::size_t x = 0; x < o.width; ++x) {
      image->setPixel(x, y, o.getPixelFromBuffer(buffer, 0, x, y));
    }
  }
};

const ColorMode::UpdateAllColorsFunction ColorMode::MonochromeUpdateAllColors =
    [](Optional<OutputComponent *> memoryAccess,
       const Options &o,
       std::shared_ptr<QImage> image) -> void {
  std::size_t colorCount = 2;
  std::size_t cellSize = 8;    // TODO
  std::size_t pointerSize = 32;// TODO
  constexpr std::size_t colorSizeInBit = 32;
  std::size_t colorSize = (colorSizeInBit + cellSize - 1) / cellSize;
  std::size_t colorTablePointer = o.colorBaseAddress;
  if (o.colorTablePointerLike) {
    MemoryValue colorTableBaseAddress{pointerSize};
    if (memoryAccess) {
      colorTableBaseAddress =
          (*memoryAccess)
              ->getMemoryAccess()
              .getMemoryValueAt(colorTablePointer,
                                (pointerSize + cellSize - 1) / cellSize)
              .get();
    }
    colorTablePointer = conversions::convert<std::size_t>(
        colorTableBaseAddress, conversions::standardConversions::nonsigned);
  }
  MemoryValue buffer{colorSize * cellSize * colorCount};
  if (memoryAccess) {
    buffer = (*memoryAccess)
                 ->getMemoryAccess()
                 .getMemoryValueAt(colorTablePointer, colorSize * colorCount)
                 .get();
  }
  for (std::size_t i = 0; i < colorCount; ++i) {
    image->setColor(i, o.getColorFromBuffer(buffer, 0, i));
  }
};

// ColorMode*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*
ColorMode Options::RGB{ColorMode::RGBGetPixel,
                       ColorMode::RGBGetColor,
                       ColorMode::RGBGetPixelFromBuffer,
                       ColorMode::RGBGetColorFromBuffer,
                       ColorMode::RGBUpdateMemory,
                       ColorMode::RGBUpdateAllPixels,
                       ColorMode::RGBUpdateAllColors};
ColorMode Options::Monochrome{ColorMode::MonochromeGetPixel,
                              ColorMode::MonochromeGetColor,
                              ColorMode::MonochromeGetPixelFromBuffer,
                              ColorMode::MonochromeGetColorFromBuffer,
                              ColorMode::MonochromeUpdateMemory,
                              ColorMode::MonochromeUpdateAllPixels,
                              ColorMode::MonochromeUpdateAllColors};
}
