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

namespace colorMode {
struct ColorMode;
struct Options {
  std::size_t pixelBaseAddress = 0;
  std::size_t colorBaseAddress = 0;
  std::size_t width = 320;
  std::size_t height = 240;
  std::size_t colorMode = 1;
  std::size_t rBit = 8;
  std::size_t gBit = 8;
  std::size_t bBit = 8;
  // TODO::use enums, maybe
  bool columns_rows = false;// false->row_major, true->columns->major
  bool horizontallyMirrored = false;
  bool verticallyMirrored = false;
  bool tight = false;
  bool colorTablePointerLike = true;
  std::size_t freeBytes = 1;

  ColorMode getColorMode() const;
  std::uint32_t getPixel(std::size_t x, std::size_t y) const;
  std::uint32_t getColor(std::size_t index) const;
  void updatePixel(std::shared_ptr<QImage> image,
                   std::size_t x,
                   std::size_t y) const;
  void updateColor(std::shared_ptr<QImage> image, std::size_t index) const;
  void updateMemory(std::shared_ptr<QImage> image,
                    std::size_t address,
                    std::size_t amount) const;
  void updateAllPixels(std::shared_ptr<QImage> image) const;
  void updateAllColors(std::shared_ptr<QImage> image) const;
  static ColorMode RGB;
  static ColorMode Monochrome;
};
struct ColorMode {
  using GetPixelFunction =
      std::function<std::uint32_t(const Options &, std::size_t, std::size_t)>;
  using GetColorFunction =
      std::function<std::uint32_t(const Options &, std::size_t)>;
  using UpdateMemoryFunction = std::function<void(
      const Options &, std::shared_ptr<QImage>, std::size_t, std::size_t)>;
  using UpdateAllPixelsFunction =
      std::function<void(const Options &, std::shared_ptr<QImage>)>;
  using UpdateAllColorsFunction =
      std::function<void(const Options &, std::shared_ptr<QImage>)>;

  GetPixelFunction getPixel;
  GetColorFunction getColor;
  UpdateMemoryFunction updateMemory;
  UpdateAllPixelsFunction updateAllPixels;
  UpdateAllColorsFunction updateAllColors;
  // RGB:
  const static GetPixelFunction RGBGetPixel;
  const static GetColorFunction RGBGetColor;
  const static UpdateMemoryFunction RGBUpdateMemory;
  const static UpdateAllPixelsFunction RGBUpdateAllPixels;
  const static UpdateAllColorsFunction RGBUpdateAllColors;
  // Monochrome
  const static GetPixelFunction MonochromeGetPixel;
  const static GetColorFunction MonochromeGetColor;
  const static UpdateMemoryFunction MonochromeUpdateMemory;
  const static UpdateAllPixelsFunction MonochromeUpdateAllPixels;
  const static UpdateAllColorsFunction MonochromeUpdateAllColors;
};
}

class PixelDisplayPaintedItem : public QQuickPaintedItem {
  Q_PROPERTY(void paint)
 public:
  PixelDisplayPaintedItem(QQuickItem *parent = 0) : QQuickPaintedItem(parent) {
    _image = std::make_shared<QImage>(_breadth, _height, QImage::Format_Mono);
    //_image->fill(QColor("#00FFFF").rgb());
    //_image->setPixel(20, 20, 0xFF0000u);
  }

  void paint(QPainter *painter) {
    std::cout << "paint!" << std::endl;
    _options.updateAllPixels(_image);
    painter->drawImage(painter->window(), *_image);
  }

  void memoryChanged(std::size_t address, std::size_t amount);

  void redrawPixel(std::size_t x, std::size_t y);

  void redrawAll();

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
  std::size_t _breadth = 320;
  std::size_t _height = 240;
  std::size_t _colorMode = 0;

  bool isInRange(std::size_t address, std::size_t amount) const;
  std::vector<std::pair<std::size_t, std::size_t>>
  pixelsInRange(std::size_t address, std::size_t amount) const;
};

#endif// ERAGPSIM_UI_PIXEL_DISPLAY_PAINTED_ITEM_HPP
