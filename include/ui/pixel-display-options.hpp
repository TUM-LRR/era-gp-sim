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

#ifndef ERAGPSIM_UI_PIXEL_DISPLAY_OPTIONS_HPP
#define ERAGPSIM_UI_PIXEL_DISPLAY_OPTIONS_HPP

#include <memory>

#include "common/optional.hpp"

class OutputComponent;
class QImage;
class MemoryValue;

namespace colormode {
struct ColorMode;
struct Options {
  using size_t = std::size_t;
  // address of of the pixelBuffer(pointer)
  size_t pixelBaseAddress = 0;
  // address of of the colorBuffer(pointer)
  size_t colorBaseAddress = 4;
  // width of the image
  size_t width = 16;
  // height of the image
  size_t height = 16;
  // colorMode RGB:Monochrome
  size_t colorMode = 0;
  // number of bits the red part of a color in RGB mode has
  size_t rBit = 8;
  // number of bits the green part of a color in RGB mode has
  size_t gBit = 8;
  // number of bits the blue part of a color in RGB mode has
  size_t bBit = 8;
  // TODO::use enums, maybe? probably not, cause qml is annoying
  // false->row_major, true->columns->major
  bool columns_rows = false;
  // mirrors the image horizontally
  bool horizontallyMirrored = false;  // TODO
  // mirrors the image vertically
  bool verticallyMirrored = false;  // TODO
  // packs pixel data as tight as possible
  bool tight = false;
  // interpret PixelBaseAddress as pointer to the PixelBuffer
  bool pixelBufferPointerLike = true;
  // interpret ColorBaseAddress as pointer to the colorTable
  bool colorTablePointerLike = true;
  // ignore n bytes after each pixel (in RGB mode)
  size_t freeBytes = 1;
  // waste n bits in each byte (in Monochrome mode)
  size_t freeBits = 0;
  // number of possible errors
  static constexpr size_t maxError = 5;
  /* space for Errors
   * 0:couldn't access memoryAccess
   * 1:pointer outside memory
   * 2:pixelBuffer outside Memory
   * 3:colorBuffer outside Memory
   * 4:bufferStart address not in Memory
   */
  std::vector<bool> errorVector = std::vector<bool>(maxError, false);

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
  std::uint32_t
  getPixel(Optional<OutputComponent *> memoryAccess, size_t x, size_t y);
  /*
   * \brief returns the color from the color Table at the index index
   * \param memoryAccess source to load the data from
   * \param index index of the entry in the color Table
   */
  std::uint32_t
  getColor(Optional<OutputComponent *> memoryAccess, size_t index);
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
                                   size_t offset,
                                   size_t x,
                                   size_t y);
  /*
   * \brief returns the color from the color Table at the index index fetching
   *        the data from the prefetched Buffer buffer
   * \param buffer Buffer containing the color data for the color at index
   * \param offset difference between the starting address of buffer and the
   *        colorBuffer
   * \param index index of the entry in the color Table
   */
  std::uint32_t
  getColorFromBuffer(const MemoryValue &buffer, size_t offset, size_t index);
  /*
   * \brief redraws the Pixel at (x,y)
   * \param memoryAccess source to load the data from
   * \param image the image to be graced with the pixel at (x,y)
   * \param x X position of the Pixel
   * \param y Y position of the Pixel
   */
  void updatePixel(Optional<OutputComponent *> memoryAccess,
                   std::shared_ptr<QImage> image,
                   size_t x,
                   size_t y);
  /*
   * \brief updates the color from the color Table at the index index
   * \param memoryAccess source to load the data from
   * \param image Image whichs color table at entry index should be updated
   * \param index index of the entry in the color Table
   */
  void updateColor(Optional<OutputComponent *> memoryAccess,
                   std::shared_ptr<QImage> image,
                   size_t index);
  /*
   * \brief updates all pixels of the image
   * \param memoryAccess source to load the data from
   * \param image Image to be be updated
   * \param address begin address in the memory that has changed
   * \param amount length of the area that has changed memory cells in it
   */
  void updateMemory(Optional<OutputComponent *> memoryAccess,
                    std::shared_ptr<QImage> image,
                    size_t address,
                    size_t amount);
  /*
   * \brief updates all pixels of the image
   * \param memoryAccess source to load the data from
   * \param image Image to be be updated
   */
  void updateAllPixels(Optional<OutputComponent *> memoryAccess,
                       std::shared_ptr<QImage> image);
  /*
   * \brief updates all colors of the image
   * \param memoryAccess source to load the data from
   * \param image Image to be be updated
   */
  void updateAllColors(Optional<OutputComponent *> memoryAccess,
                       std::shared_ptr<QImage> image);

  /*
   * \brief sets the error with the index index to true
   * \param index index of the error in the error vector
   */
  void setError(size_t index);

  /*
   * \brief Handles all errors and resets the error vector
   */
  void handleErrors();

  std::shared_ptr<QImage> makeImage() const;
  // The color mode used for RGB Color Mode
  static ColorMode RGB;
  // The color mode used for Monochrome ColorMode
  static ColorMode Monochrome;
};
}

#endif  // ERAGPSIM_UI_PIXEL_DISPLAY_OPTIONS_HPP
