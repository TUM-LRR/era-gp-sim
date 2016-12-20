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

#include "common/optional.hpp"
//#include "core/memory-access.hpp"
//#include "ui/output-component.hpp"
class OutputComponent;
class MemoryValue;

#define PSIZE 16
#define CMODE 1

namespace colorMode {
struct ColorMode;
struct Options {
  std::size_t pixelBaseAddress = 0;
  std::size_t colorBaseAddress = 4;
  std::size_t width = PSIZE;
  std::size_t height = PSIZE;
  std::size_t colorMode = CMODE;
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
  bool pixelBufferPointerLike = true;
  // interpret ColorBaseAddress as pointer to the colorTable
  bool colorTablePointerLike = true;
  std::size_t freeBytes = 1;
  std::size_t freeBits = 0;

  /*
   * \brief returns the current ColorMode
   */
  ColorMode getColorMode() const;
  /*
   * \brief returns the color of the pixel at (x,y)
   * \param memoryAccess source to load the data from
   * \param x X position of the Pixel
   * \param y Y position of the Pixel
   */
  std::uint32_t getPixel(Optional<OutputComponent *> memoryAccess,
                         std::size_t x,
                         std::size_t y) const;
  /*
   * \brief returns the color from the color Table at the index index
   * \param memoryAccess source to load the data from
   * \param index index of the entry in the color Table
   */
  std::uint32_t
  getColor(Optional<OutputComponent *> memoryAccess, std::size_t index) const;
  /*
   * \brief returns the color of the pixel at (x,y) fetching the data from the
   *        prefetched Buffer buffer
   * \param buffer Buffer containing the pixel data for the to be drawn Pixel
   * \param offset difference between the starting address of buffer and the
   *        pixelBuffer
   * \param x X position of the Pixel
   * \param y Y position of the Pixel
   */
  std::uint32_t getPixelFromBuffer(const MemoryValue &buffer,
                                   std::size_t offset,
                                   std::size_t x,
                                   std::size_t y) const;
  /*
   * \brief returns the color from the color Table at the index index fetching
   *        the data from the prefetched Buffer buffer
   * \param buffer Buffer containing the color data for the color at index
   * \param offset difference between the starting address of buffer and the
   *        colorBuffer
   * \param index index of the entry in the color Table
   */
  std::uint32_t getColorFromBuffer(const MemoryValue &buffer,
                                   std::size_t offset,
                                   std::size_t index) const;
  /*
   * \brief redraws the Pixel at (x,y)
   * \param memoryAccess source to load the data from
   * \param image the image to be graced with the pixel at (x,y)
   * \param x X position of the Pixel
   * \param y Y position of the Pixel
   */
  void updatePixel(Optional<OutputComponent *> memoryAccess,
                   std::shared_ptr<QImage> image,
                   std::size_t x,
                   std::size_t y) const;
  /*
   * \brief updates the color from the color Table at the index index
   * \param memoryAccess source to load the data from
   * \param image Image whichs color table at entry index should be updated
   * \param index index of the entry in the color Table
   */
  void updateColor(Optional<OutputComponent *> memoryAccess,
                   std::shared_ptr<QImage> image,
                   std::size_t index) const;
  /*
   * \brief updates all pixels of the image
   * \param memoryAccess source to load the data from
   * \param image Image to be be updated
   * \param address begin address in the memory that has changed
   * \param amount length of the area that has changed memory cells in it
   */
  void updateMemory(Optional<OutputComponent *> memoryAccess,
                    std::shared_ptr<QImage> image,
                    std::size_t address,
                    std::size_t amount) const;
  /*
   * \brief updates all pixels of the image
   * \param memoryAccess source to load the data from
   * \param image Image to be be updated
   */
  void updateAllPixels(Optional<OutputComponent *> memoryAccess,
                       std::shared_ptr<QImage> image) const;
  /*
   * \brief updates all colors of the image
   * \param memoryAccess source to load the data from
   * \param image Image to be be updated
   */
  void updateAllColors(Optional<OutputComponent *> memoryAccess,
                       std::shared_ptr<QImage> image) const;

  std::shared_ptr<QImage> makeImage() const;
  // The color mode used for RGB Color Mode
  static ColorMode RGB;
  // The color mode used for Monochrome ColorMode
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

  /*
   * \brief returns the color of the pixel at (x,y)
   */
  GetPixelFunction getPixel;
  /*
   * \brief returns the color from the color Table at the index index
   */
  GetColorFunction getColor;
  /*
   * \brief returns the color of the pixel at (x,y) fetching the data from the
   *        prefetched Buffer buffer
   */
  GetPixelFromBufferFunction getPixelFromBuffer;
  /*
   * \brief returns the color from the color Table at the index index fetching
   *        the data from the prefetched Buffer buffer
   */
  GetColorFromBufferFunction getColorFromBuffer;
  /*
   * \brief updates the image wherever the given change in memory modified the
   *        image
   */
  UpdateMemoryFunction updateMemory;
  /*
   * \brief updates all pixels of the image
   */
  UpdateAllPixelsFunction updateAllPixels;
  /*
   * \brief updates all colors of the image
   */
  UpdateAllColorsFunction updateAllColors;

  static std::size_t loadPointer(Optional<OutputComponent *> memoryAccess,
                                 std::size_t address,
                                 bool indirect,
                                 std::size_t cellSize,
                                 std::size_t pointerSize);
  static MemoryValue getMemoryValueAt(Optional<OutputComponent *> memoryAccess,
                                      std::size_t address,
                                      std::size_t length,
                                      std::size_t defaultLength = 1);
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
  // Q_PROPERTY(std::size_t pixelBaseAddress READ getPixelBaseAddress WRITE\
  //                setPixelBaseAddress)
  // Q_PROPERTY(std::size_t colorBaseAddress READ getColorBaseAddress WRITE\
  //                setColorBaseAddress)
  // Q_PROPERTY(std::size_t width READ getWidth WRITE setWidth)
  // Q_PROPERTY(std::size_t height READ getHeight WRITE setHeight)
  // Q_PROPERTY(std::size_t colorMode READ getColorMode WRITE setColorMode)
  // Q_PROPERTY(std::size_t rBit READ getRBit WRITE setRBit)
  // Q_PROPERTY(std::size_t gBit READ getGBit WRITE setGBit)
  // Q_PROPERTY(std::size_t bBit READ getBBit WRITE setBBit)
  Q_PROPERTY(bool columns_rows READ getColumns_rows WRITE setColumns_rows)
  // Q_PROPERTY(bool horizontallyMirrored READ getHorizontallyMirrored WRITE\
  //                setHorizontallyMirrored)
  // Q_PROPERTY(bool verticallyMirrored READ getVerticallyMirrored WRITE\
  //                setVerticallyMirrored)
  // Q_PROPERTY(bool tight READ getTight WRITE setTight)
  // Q_PROPERTY(bool pixelBufferPointerLike READ getPixelBufferPointerLike WRITE\
  //                setPixelBufferPointerLike)
  // Q_PROPERTY(bool colorTablePointerLike READ getColorTablePointerLike WRITE\
  //                setColorTablePointerLike)
  // Q_PROPERTY(std::size_t freeBytes READ getFreeBytes WRITE setFreeBytes)
  // Q_PROPERTY(std::size_t freeBits READ getFreeBits WRITE setFreeBits)

 public:
  /*
   * \brief constructs a new PixelDisplayPaintedItem
   * \param parent the parent
   */
  PixelDisplayPaintedItem(QQuickItem *parent = 0);

  /*
   * \brief paints the image held by this onto the painter
   * \param painter the painter to be drawn upon
   */
  void paint(QPainter *painter);

  /*
   * \brief redraws the necessary pixels for the changed bytes in memory
   * \param address first byte that was modified
   * \param amount length of the area of modified bytes
   */
  Q_INVOKABLE void memoryChanged(std::size_t address, std::size_t amount);

  /*
   * \brief sets the size of the image
   * \param width new width of the image
   * \param width new height of the image
   */
  void resize(std::size_t width, std::size_t height);

  /*
   * \brief sets the OutputComponent
   * \param o pointer to the OutputComponent
   */
  void setOutputComponent(OutputComponent *o);

  void setPixelBaseAddress(std::size_t pixelBaseAddress);
  void setColorBaseAddress(std::size_t colorBaseAddress);
  void setWidth(std::size_t width);
  void setHeight(std::size_t height);
  /*
   * \brief sets the colorMode
   * \param colorMode The colorMode to be set active
   * \Note I maybe should use enums but enums in qt are annoying
   */
  void setColorMode(std::size_t colorMode);
  void setRBit(std::size_t rBit);
  void setGBit(std::size_t gBit);
  void setBBit(std::size_t bBit);
  void setColumns_rows(bool columns_rows);
  void setHorizontallyMirrored(bool horizontallyMirrored);
  void setVerticallyMirrored(bool verticallyMirrored);
  void setTight(bool tight);
  void setPixelBufferPointerLike(bool pixelBufferPointerLike);
  void setColorTablePointerLike(bool colorTablePointerLike);
  void setFreeBytes(std::size_t freeBytes);
  void setFreeBits(std::size_t freeBits);

  std::size_t getPixelBaseAddress();
  std::size_t getColorBaseAddress();
  std::size_t getWidth();
  std::size_t getHeight();
  std::size_t getColorMode();
  std::size_t getRBit();
  std::size_t getGBit();
  std::size_t getBBit();
  bool getColumns_rows();
  bool getHorizontallyMirrored();
  bool getVerticallyMirrored();
  bool getTight();
  bool getPixelBufferPointerLike();
  bool getColorTablePointerLike();
  std::size_t getFreeBytes();
  std::size_t getFreeBits();

 private:
  /// shared pointer to the image
  std::shared_ptr<QImage> _image;
  /// struct storing the options used to draw the image
  colorMode::Options _options;
  /// pointer to the OutputComponent used to get the memoryAccess
  Optional<OutputComponent *> _outputComponentPointer;
};

#endif// ERAGPSIM_UI_PIXEL_DISPLAY_PAINTED_ITEM_HPP
