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

#include "ui/pixel-display-color-mode.hpp"

#include <QImage>

#include "core/conversions.hpp"
#include "core/memory-value.hpp"
#include "ui/output-component.hpp"
#include "ui/pixel-display-options.hpp"

namespace colormode {
size_t ColorMode::loadPointer(Optional<OutputComponent *> memoryAccess,
                              size_t address,
                              bool indirect,
                              size_t cellSize,
                              size_t pointerSize,
                              Options &o) {
  if (indirect) {
    if (memoryAccess) {
      if ((address + (pointerSize + cellSize - 1) / cellSize) >=
          (*memoryAccess)->getMemoryAccess().getMemorySize().get()) {
        // the address of the pointer is invalid
        o.setError(1);
        return 0;
      }
      MemoryValue pointer = ColorMode::getMemoryValueAt(
          memoryAccess, address, (pointerSize + cellSize - 1) / cellSize, o);
      return conversions::convert<size_t>(
          pointer, conversions::standardConversions::nonsigned);
    } else {
      return 0;
    }
  } else {
    return address;
  }
}
MemoryValue
ColorMode::getMemoryValueAt(Optional<OutputComponent *> memoryAccess,
                            size_t address,
                            size_t length,
                            // size_t defaultLength,
                            Options &o) {
  if (memoryAccess) {
    size_t memSize = (*memoryAccess)->getMemoryAccess().getMemorySize().get();
    if (memSize >= address + length) {
      return (*memoryAccess)
          ->getMemoryAccess()
          .tryGetMemoryValueAt(address, length)
          .get();
    } else if (memSize > address) {
      return (*memoryAccess)
          ->getMemoryAccess()
          .tryGetMemoryValueAt(address, memSize - address)
          .get();
    } else {
      o.setError(4);
      return MemoryValue{1};
    }
  }
  o.setError(0);
  return MemoryValue{1};
}


// RGB*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*
const ColorMode::GetPixelFunction ColorMode::RGBGetPixel = [](
    Optional<OutputComponent *> memoryAccess,
    Options &o,
    size_t x,
    size_t y) -> std::uint32_t {
  size_t cellSize = 8;      // TODO
  size_t pointerSize = 32;  // TODO
  size_t pixelBufferPointer = ColorMode::loadPointer(memoryAccess,
                                                     o.pixelBaseAddress,
                                                     o.pixelBufferPointerLike,
                                                     cellSize,
                                                     pointerSize,
                                                     o);
  size_t sizeInBit = o.rBit + o.gBit + o.bBit;
  size_t byteSize =
      (sizeInBit + cellSize - 1) / cellSize + (o.tight ? 0 : o.freeBytes);
  size_t index =
      x * (o.columns_rows ? o.height : 1) + y * (o.columns_rows ? 1 : o.width);
  size_t address = pixelBufferPointer;
  size_t bitOffset = 0;
  if (o.tight) {
    address += (sizeInBit * index) / cellSize;
    bitOffset = (sizeInBit * index) % cellSize;
  } else {
    address += byteSize * index;
  }
  MemoryValue buffer = ColorMode::getMemoryValueAt(
      memoryAccess, address, byteSize + (bitOffset > 0 ? 1 : 0), o);
  return o.getPixelFromBuffer(buffer, address, x, y);
};

const ColorMode::GetColorFunction ColorMode::RGBGetColor = [](
    Optional<OutputComponent *> memoryAccess,
    Options &o,
    size_t index) -> std::uint32_t { return 0; };

const ColorMode::GetPixelFromBufferFunction ColorMode::RGBGetPixelFromBuffer =
    [](const MemoryValue &buffer,
       size_t offset,
       Options &o,
       size_t x,
       size_t y) -> std::uint32_t {
  size_t cellSize = 8;  // TODO
  size_t sizeInBit = o.rBit + o.gBit + o.bBit;
  size_t byteSize =
      (sizeInBit + cellSize - 1) / cellSize + (o.tight ? 0 : o.freeBytes);
  size_t index =
      x * (o.columns_rows ? o.height : 1) + y * (o.columns_rows ? 1 : o.width);
  size_t beginBit = index * (o.tight ? sizeInBit : (byteSize * cellSize));
  size_t bitOffset = beginBit - (offset * cellSize);
  // Load from buffer!
  MemoryValue mem{sizeInBit};
  if (buffer.getSize() > bitOffset + sizeInBit) {
    mem = buffer.subSet(bitOffset, bitOffset + sizeInBit);
  } else {
    // the pixel is apparently not within the memory -> show error color
    o.setError(2);
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
       size_t offset,
       Options &o,
       size_t index) -> std::uint32_t { return 0; };

const ColorMode::UpdateMemoryFunction ColorMode::RGBUpdateMemory = [](
    Optional<OutputComponent *> memoryAccess,
    Options &o,
    std::shared_ptr<QImage> image,
    size_t address,
    size_t amount) -> void {
  size_t cellSize = 8;      // TODO
  size_t pointerSize = 32;  // TODO
  size_t memSize = (*memoryAccess)->getMemoryAccess().getMemorySize().get();
  size_t pointerSizeInByte = (pointerSize + cellSize - 1) / cellSize;
  size_t pixelBufferPointer = o.pixelBaseAddress;
  bool hasAlreadyUpdatedAllPixels = false;
  if (o.pixelBufferPointerLike) {
    if (address < pixelBufferPointer + pointerSizeInByte &&
        address + amount > pixelBufferPointer) {
      // pixel pointer has changed
      o.updateAllPixels(memoryAccess, image);
      hasAlreadyUpdatedAllPixels = true;
    } else {
      pixelBufferPointer = ColorMode::loadPointer(memoryAccess,
                                                  o.pixelBaseAddress,
                                                  o.pixelBufferPointerLike,
                                                  cellSize,
                                                  pointerSize,
                                                  o);
    }
  }
  if (!hasAlreadyUpdatedAllPixels) {
    size_t sizeInBit = o.rBit + o.gBit + o.bBit;
    size_t pixelBufferSize =
        (o.tight ? ((sizeInBit * o.width * o.height + cellSize - 1) / cellSize)
                 : (((sizeInBit + cellSize - 1) / cellSize + o.freeBytes) *
                    o.height * o.width));
    if (pixelBufferPointer + pixelBufferSize > memSize) {
      o.setError(2);
    }
    if (address < pixelBufferPointer + pixelBufferSize &&
        address + amount > pixelBufferPointer) {
      // At least some pixel have changed
      size_t sizeInByte =
          (sizeInBit + cellSize - 1) / cellSize + (o.tight ? 0 : o.freeBytes);
      size_t minIndex = 0;
      if (address > pixelBufferPointer) {
        minIndex = o.tight
                       ? ((address - pixelBufferPointer) * cellSize / sizeInBit)
                       : ((address - pixelBufferPointer) / sizeInByte);
      }
      size_t maxIndex = o.height * o.width;
      if (address + amount < pixelBufferPointer + pixelBufferSize) {
        maxIndex =
            o.tight
                ? (((address + amount - pixelBufferPointer) * cellSize +
                    sizeInBit - 1) /
                   sizeInBit)
                : (((address + amount - pixelBufferPointer) + sizeInByte - 1) /
                   sizeInByte);
      }
      size_t beginOffset =
          o.tight ? (minIndex * sizeInBit / cellSize) : (minIndex * sizeInByte);
      size_t endOffset =
          o.tight ? (maxIndex * sizeInBit / cellSize) : (maxIndex * sizeInByte);
      MemoryValue buffer =
          ColorMode::getMemoryValueAt(memoryAccess,
                                      pixelBufferPointer + beginOffset,
                                      endOffset - beginOffset,
                                      o);
      for (size_t index = minIndex; index < maxIndex; index++) {
        size_t x = o.columns_rows ? (index / o.height) : (index % o.width);
        size_t y = o.columns_rows ? (index % o.height) : (index / o.width);
        image->setPixel(x, y, o.getPixelFromBuffer(buffer, beginOffset, x, y));
      }
    }
  }
};

const ColorMode::UpdateAllPixelsFunction ColorMode::RGBUpdateAllPixels = [](
    Optional<OutputComponent *> memoryAccess,
    Options &o,
    std::shared_ptr<QImage> image) -> void {
  size_t cellSize = 8;      // TODO
  size_t pointerSize = 32;  // TODO
  size_t pixelBufferPointer = ColorMode::loadPointer(memoryAccess,
                                                     o.pixelBaseAddress,
                                                     o.pixelBufferPointerLike,
                                                     cellSize,
                                                     pointerSize,
                                                     o);
  size_t sizeOfColorInBit = o.rBit + o.gBit + o.bBit;
  if (!o.tight) {
    sizeOfColorInBit =
        ((sizeOfColorInBit + cellSize - 1) / cellSize + o.freeBytes) * cellSize;
  }
  size_t sizeInMemory =
      (sizeOfColorInBit * o.width * o.height + cellSize - 1) / cellSize;
  MemoryValue buffer = ColorMode::getMemoryValueAt(
      memoryAccess, pixelBufferPointer, sizeInMemory, o);
  for (size_t y = 0; y < o.height; ++y) {
    for (size_t x = 0; x < o.width; ++x) {
      image->setPixel(x, y, o.getPixelFromBuffer(buffer, 0, x, y));
    }
  }
};

const ColorMode::UpdateAllColorsFunction ColorMode::RGBUpdateAllColors = [](
    Optional<OutputComponent *> memoryAccess,
    Options &o,
    std::shared_ptr<QImage> image) -> void {};

const ColorMode::CheckErrorsFunction ColorMode::RGBCheckErrors = [](
    Optional<OutputComponent *> memoryAccess, Options &o) -> void {
  size_t pointerSize = 4;
  if (memoryAccess) {
    size_t memSize = (*memoryAccess)->getMemoryAccess().getMemorySize().get();
    if (o.pixelBaseAddress + pointerSize >= memSize) {
      o.setError(1);
      return;
    }
  } else {
    o.setError(0);
  }
};

// Monochrome*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*
const ColorMode::GetPixelFunction ColorMode::MonochromeGetPixel = [](
    Optional<OutputComponent *> memoryAccess,
    Options &o,
    size_t x,
    size_t y) -> std::uint32_t {
  size_t cellSize = 8;            // TODO
  size_t bitsPerByte = cellSize;  // TODO
  size_t index =
      x * (o.columns_rows ? o.height : 1) + y * (o.columns_rows ? 1 : o.width);
  size_t address = o.pixelBaseAddress + index / bitsPerByte;
  MemoryValue buffer = ColorMode::getMemoryValueAt(memoryAccess, address, 1, o);
  return o.getPixelFromBuffer(buffer, address, x, y);
};
const ColorMode::GetColorFunction ColorMode::MonochromeGetColor = [](
    Optional<OutputComponent *> memoryAccess,
    Options &o,
    size_t index) -> std::uint32_t {
  size_t cellSize = 8;      // TODO
  size_t pointerSize = 32;  // TODO
  constexpr size_t colorSizeInBit = 32;
  size_t colorSize = (32 + cellSize - 1) / cellSize;
  size_t colorTablePointer = ColorMode::loadPointer(memoryAccess,
                                                    o.colorBaseAddress,
                                                    o.colorTablePointerLike,
                                                    cellSize,
                                                    pointerSize,
                                                    o);
  MemoryValue buffer = ColorMode::getMemoryValueAt(
      memoryAccess, colorTablePointer + (index * colorSize), colorSize, o);
  return o.getColorFromBuffer(buffer, index * colorSize, index);
};

const ColorMode::GetPixelFromBufferFunction
    ColorMode::MonochromeGetPixelFromBuffer = [](const MemoryValue &buffer,
                                                 size_t offset,
                                                 Options &o,
                                                 size_t x,
                                                 size_t y) -> std::uint32_t {
  size_t cellSize = 8;  // TODO
  size_t bitsPerByte = cellSize - o.freeBits;
  size_t index =
      x * (o.columns_rows ? o.height : 1) + y * (o.columns_rows ? 1 : o.width);
  size_t address = (index / bitsPerByte * cellSize) + (index % bitsPerByte);
  if (buffer.getSize() > address) {
    return buffer.get(address) ? 1 : 0;
  }
  o.setError(2);
  return 0;
};
const ColorMode::GetColorFromBufferFunction
    ColorMode::MonochromeGetColorFromBuffer =
        [](const MemoryValue &buffer,
           size_t offset,
           Options &o,
           size_t index) -> std::uint32_t {
  size_t cellSize = 8;  // TODO
  constexpr size_t colorSizeInBit = 32;
  size_t colorSize = (colorSizeInBit + cellSize - 1) / cellSize;
  size_t byteOffset = (colorSize * index) - offset;
  if (buffer.getSize() >= byteOffset * cellSize + colorSizeInBit) {
    MemoryValue color = buffer.subSet(byteOffset * cellSize,
                                      byteOffset * cellSize + colorSizeInBit);
    return conversions::convert<std::uint32_t>(
        color, conversions::standardConversions::nonsigned);
  }
  o.setError(3);
  return 0xFFFF00FF;  // error color
};

const ColorMode::UpdateMemoryFunction ColorMode::MonochromeUpdateMemory = [](
    Optional<OutputComponent *> memoryAccess,
    Options &o,
    std::shared_ptr<QImage> image,
    size_t address,
    size_t amount) -> void {
  size_t cellSize = 8;      // TODO
  size_t pointerSize = 32;  // TODO
  size_t memSize = (*memoryAccess)->getMemoryAccess().getMemorySize().get();
  size_t pointerSizeInByte = (pointerSize + cellSize - 1) / cellSize;
  size_t pixelBufferPointer = o.pixelBaseAddress;
  bool hasAlreadyUpdatedAllPixels = false;
  if (o.pixelBufferPointerLike) {
    if (address < pixelBufferPointer + pointerSizeInByte &&
        address + amount > pixelBufferPointer) {
      // pixel pointer has changed
      o.updateAllPixels(memoryAccess, image);
      hasAlreadyUpdatedAllPixels = true;
    } else {
      pixelBufferPointer = ColorMode::loadPointer(memoryAccess,
                                                  o.pixelBaseAddress,
                                                  o.pixelBufferPointerLike,
                                                  cellSize,
                                                  pointerSize,
                                                  o);
    }
  }
  if (!hasAlreadyUpdatedAllPixels) {
    size_t pixelBufferSize = (o.width * o.height + cellSize - 1) / cellSize;
    if (pixelBufferPointer + pixelBufferSize > memSize) {
      o.setError(2);
    }
    if (address < pixelBufferPointer + pixelBufferSize &&
        address + amount > pixelBufferPointer) {
      // At least some pixel have changed
      size_t bitsPerByte = cellSize - o.freeBits;
      size_t minIndex = 0;
      if (address > pixelBufferPointer) {
        minIndex = (address - pixelBufferPointer) * bitsPerByte;
      }
      size_t maxIndex = o.height * o.width;
      if (address + amount < pixelBufferPointer + pixelBufferSize) {
        maxIndex = (address + amount - pixelBufferPointer) * bitsPerByte;
      }
      size_t beginOffset = minIndex / bitsPerByte;
      size_t endOffset = (maxIndex + bitsPerByte - 1) / bitsPerByte;
      MemoryValue buffer = ColorMode::getMemoryValueAt(
          memoryAccess, pixelBufferPointer, pixelBufferSize, o);
      // this could be optimized, we don't actually need the whole pixel buffer,
      // but anyways... (I'm not sure I'm calculating the offset correctly TODO)
      //  .getMemoryValueAt(pixelBufferPointer + beginOffset,
      //                    endOffset - beginOffset)
      for (size_t index = minIndex; index < maxIndex; index++) {
        size_t x = o.columns_rows ? (index / o.height) : (index % o.width);
        size_t y = o.columns_rows ? (index % o.height) : (index / o.width);
        image->setPixel(x, y, o.getPixelFromBuffer(buffer, 0, x, y));
      }
    }
  }
  // the colors
  size_t colorBufferPointer = o.colorBaseAddress;
  bool hasAlreadyUpdatedAllColors = false;
  if (o.colorTablePointerLike) {
    if (address < colorBufferPointer + pointerSizeInByte &&
        address + amount > colorBufferPointer) {
      // pixel pointer has changed
      o.updateAllColors(memoryAccess, image);
      hasAlreadyUpdatedAllColors = true;
    } else {
      colorBufferPointer = ColorMode::loadPointer(memoryAccess,
                                                  o.colorBaseAddress,
                                                  o.colorTablePointerLike,
                                                  cellSize,
                                                  pointerSize,
                                                  o);
    }
  }
  if (!hasAlreadyUpdatedAllColors) {
    constexpr size_t colorCount = 2;
    constexpr size_t colorSizeInBit = 32;
    size_t colorSizeInByte = (colorSizeInBit + cellSize - 1) / cellSize;
    size_t colorBufferSize = colorCount * colorSizeInByte;
    if (colorBufferPointer + colorBufferSize > memSize) {
      o.setError(3);
    }
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
       Options &o,
       std::shared_ptr<QImage> image) -> void {
  size_t cellSize = 8;      // TODO
  size_t pointerSize = 32;  // TODO
  size_t pixelBufferPointer = ColorMode::loadPointer(memoryAccess,
                                                     o.pixelBaseAddress,
                                                     o.pixelBufferPointerLike,
                                                     cellSize,
                                                     pointerSize,
                                                     o);
  size_t bitsPerByte = cellSize - o.freeBits;
  size_t sizeInMemory = (o.width * o.height + bitsPerByte - 1) / bitsPerByte;
  MemoryValue buffer = ColorMode::getMemoryValueAt(
      memoryAccess, pixelBufferPointer, sizeInMemory, o);
  for (size_t y = 0; y < o.height; ++y) {
    for (size_t x = 0; x < o.width; ++x) {
      image->setPixel(x, y, o.getPixelFromBuffer(buffer, 0, x, y));
    }
  }
};

const ColorMode::UpdateAllColorsFunction ColorMode::MonochromeUpdateAllColors =
    [](Optional<OutputComponent *> memoryAccess,
       Options &o,
       std::shared_ptr<QImage> image) -> void {
  size_t colorCount = 2;
  size_t cellSize = 8;      // TODO
  size_t pointerSize = 32;  // TODO
  constexpr size_t colorSizeInBit = 32;
  size_t colorSize = (colorSizeInBit + cellSize - 1) / cellSize;
  size_t colorTablePointer = ColorMode::loadPointer(memoryAccess,
                                                    o.colorBaseAddress,
                                                    o.colorTablePointerLike,
                                                    cellSize,
                                                    pointerSize,
                                                    o);
  MemoryValue buffer = ColorMode::getMemoryValueAt(
      memoryAccess, colorTablePointer, colorSize * colorCount, o);
  for (size_t i = 0; i < colorCount; ++i) {
    image->setColor(i, o.getColorFromBuffer(buffer, 0, i));
  }
};

const ColorMode::CheckErrorsFunction ColorMode::MonochromeCheckErrors = [](
    Optional<OutputComponent *> memoryAccess, Options &o) -> void {
  size_t pointerSize = 4;
  if (memoryAccess) {
    size_t memSize = (*memoryAccess)->getMemoryAccess().getMemorySize().get();
    if (o.pixelBaseAddress + pointerSize >= memSize ||
        o.colorBaseAddress + pointerSize >= memSize) {
      o.setError(1);
      return;
    }
  } else {
    o.setError(0);
  }
};

// ColorMode*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*-_-*
ColorMode Options::RGB{ColorMode::RGBGetPixel,
                       ColorMode::RGBGetColor,
                       ColorMode::RGBGetPixelFromBuffer,
                       ColorMode::RGBGetColorFromBuffer,
                       ColorMode::RGBUpdateMemory,
                       ColorMode::RGBUpdateAllPixels,
                       ColorMode::RGBUpdateAllColors,
                       ColorMode::RGBCheckErrors};
ColorMode Options::Monochrome{ColorMode::MonochromeGetPixel,
                              ColorMode::MonochromeGetColor,
                              ColorMode::MonochromeGetPixelFromBuffer,
                              ColorMode::MonochromeGetColorFromBuffer,
                              ColorMode::MonochromeUpdateMemory,
                              ColorMode::MonochromeUpdateAllPixels,
                              ColorMode::MonochromeUpdateAllColors,
                              ColorMode::MonochromeCheckErrors};
}
