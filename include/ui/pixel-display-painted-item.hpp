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

#ifndef ERAGPSIM_UI_PIXEL_DISPLAY_PAINTED_ITEM_HPP
#define ERAGPSIM_UI_PIXEL_DISPLAY_PAINTED_ITEM_HPP

#include <QImage>
#include <QPainter>
#include <QQuickPaintedItem>
#include <QRect>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include <iostream>
#include "common/optional.hpp"
//#include "core/memory-access.hpp"
#include "ui/output-component.hpp"

namespace colorMode {
struct ColorMode;
struct Options {
  std::size_t pixelBaseAddress = 0;
  std::size_t colorBaseAddress = 0;
  std::size_t width = 32;
  std::size_t height = 24;
  std::size_t colorMode = 1;
  std::size_t rBit = 8;
  std::size_t gBit = 8;
  std::size_t bBit = 8;
  // TODO::use enums, maybe
  // false->row_major, true->columns->major
  bool columns_rows = false;
  // mirrors the image horizontally
  bool horizontallyMirrored = false;// TODO
  // mirrors the image vertically
  bool verticallyMirrored = false;// TODO
  // packs pixel data as tight as possible
  bool tight = false;
  // interpret PixelBaseAddress as pointer to the PixelBuffer
  bool pixelBufferPointerLike = false;
  // interpret ColorBaseAddress as pointer to the colorTable
  bool colorTablePointerLike  = false;
  std::size_t freeBytes = 1;
  std::size_t freeBits = 0;

  ColorMode getColorMode() const;
  std::uint32_t getPixel(Optional<OutputComponent *> memoryAccess,
                         std::size_t x,
                         std::size_t y) const;
  std::uint32_t
  getColor(Optional<OutputComponent *> memoryAccess, std::size_t index) const;
  std::uint32_t getPixelFromBuffer(const MemoryValue &buffer,
                                   std::size_t offset,
                                   std::size_t x,
                                   std::size_t y) const;
  std::uint32_t getColorFromBuffer(const MemoryValue &buffer,
                                   std::size_t offset,
                                   std::size_t index) const;
  void updatePixel(Optional<OutputComponent *> memoryAccess,
                   std::shared_ptr<QImage> image,
                   std::size_t x,
                   std::size_t y) const;
  void updateColor(Optional<OutputComponent *> memoryAccess,
                   std::shared_ptr<QImage> image,
                   std::size_t index) const;
  void updateMemory(Optional<OutputComponent *> memoryAccess,
                    std::shared_ptr<QImage> image,
                    std::size_t address,
                    std::size_t amount) const;
  void updateAllPixels(Optional<OutputComponent *> memoryAccess,
                       std::shared_ptr<QImage> image) const;
  void updateAllColors(Optional<OutputComponent *> memoryAccess,
                       std::shared_ptr<QImage> image) const;
  static ColorMode RGB;
  static ColorMode Monochrome;
};
struct ColorMode {
  using GetPixelFunction = std::function<std::uint32_t(
      Optional<OutputComponent *>, const Options &, std::size_t, std::size_t)>;
  using GetColorFunction = std::function<std::uint32_t(
      Optional<OutputComponent *>, const Options &, std::size_t)>;
  using GetPixelFromBufferFunction =
      std::function<std::uint32_t(const MemoryValue &,
                                  std::size_t,
                                  const Options &,
                                  std::size_t,
                                  std::size_t)>;
  using GetColorFromBufferFunction = std::function<std::uint32_t(
      const MemoryValue &, std::size_t, const Options &, std::size_t)>;
  using UpdateMemoryFunction = std::function<void(Optional<OutputComponent *>,
                                                  const Options &,
                                                  std::shared_ptr<QImage>,
                                                  std::size_t,
                                                  std::size_t)>;
  using UpdateAllPixelsFunction = std::function<void(
      Optional<OutputComponent *>, const Options &, std::shared_ptr<QImage>)>;
  using UpdateAllColorsFunction = std::function<void(
      Optional<OutputComponent *>, const Options &, std::shared_ptr<QImage>)>;

  GetPixelFunction getPixel;
  GetColorFunction getColor;
  GetPixelFromBufferFunction getPixelFromBuffer;
  GetColorFromBufferFunction getColorFromBuffer;
  UpdateMemoryFunction updateMemory;
  UpdateAllPixelsFunction updateAllPixels;
  UpdateAllColorsFunction updateAllColors;
  // RGB:
  const static GetPixelFunction RGBGetPixel;
  const static GetColorFunction RGBGetColor;
  const static GetPixelFromBufferFunction RGBGetPixelFromBuffer;
  const static GetColorFromBufferFunction RGBGetColorFromBuffer;
  const static UpdateMemoryFunction RGBUpdateMemory;
  const static UpdateAllPixelsFunction RGBUpdateAllPixels;
  const static UpdateAllColorsFunction RGBUpdateAllColors;
  // Monochrome
  const static GetPixelFunction MonochromeGetPixel;
  const static GetColorFunction MonochromeGetColor;
  const static GetPixelFromBufferFunction MonochromeGetPixelFromBuffer;
  const static GetColorFromBufferFunction MonochromeGetColorFromBuffer;
  const static UpdateMemoryFunction MonochromeUpdateMemory;
  const static UpdateAllPixelsFunction MonochromeUpdateAllPixels;
  const static UpdateAllColorsFunction MonochromeUpdateAllColors;
};
}

class PixelDisplayPaintedItem : public QQuickPaintedItem {
  Q_OBJECT
  Q_PROPERTY(OutputComponent *outputComponentPointer WRITE setOutputComponent)
 public:
  // PixelDisplayPaintedItem(QQuickItem *parent = 0) : QQuickPaintedItem(parent)
  // {
  //  _image = std::make_shared<QImage>(_breadth, _height,
  //  QImage::Format_RGB32);
  PixelDisplayPaintedItem(QQuickItem *parent = 0) : QQuickPaintedItem(parent) {
    _image = std::make_shared<QImage>(_breadth, _height, QImage::Format_Mono);
    //_image->fill(QColor("#00FFFF").rgb());
    //_image->setPixel(20, 20, 0xFF0000u);
  }

  void paint(QPainter *painter) {
    std::cout << "paint!" << std::endl;
    _options.updateAllPixels(_outputComponentPointer, _image);
    _options.updateAllColors(_outputComponentPointer, _image);
    painter->drawImage(painter->window(), *_image);
    std::cout << "I did do the paint!" << std::endl;
  }

  void memoryChanged(std::size_t address, std::size_t amount);

  void redrawPixel(std::size_t x, std::size_t y);

  void redrawAll();

  void setOutputComponent(OutputComponent *o) {
    _outputComponentPointer = o;
  }

  std::uint32_t getColorAt(std::size_t address);

  std::size_t getBaseAddress() const;
  std::size_t getColorMode() const;
  std::size_t getWidth() const;
  std::size_t getHeight() const;
  std::size_t getSizeInMemory() const;
  std::size_t getCellSize() const;

 private:
  std::shared_ptr<QImage> _image;
  colorMode::Options _options{};
  std::size_t _baseAddress = 0;
  std::size_t _breadth = 32;
  std::size_t _height = 24;
  std::size_t _colorMode = 0;

  Optional<OutputComponent *> _outputComponentPointer;

  bool isInRange(std::size_t address, std::size_t amount) const;
  std::vector<std::pair<std::size_t, std::size_t>>
  pixelsInRange(std::size_t address, std::size_t amount) const;
};

#endif// ERAGPSIM_UI_PIXEL_DISPLAY_PAINTED_ITEM_HPP
