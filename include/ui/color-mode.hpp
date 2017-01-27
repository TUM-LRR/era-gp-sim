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

#ifndef ERAGPSIM_UI_PIXEL_DISPLAY_COLOR_MODE_HPP
#define ERAGPSIM_UI_PIXEL_DISPLAY_COLOR_MODE_HPP

#include <cstdint>
#include <functional>
#include <memory>

#include "common/optional.hpp"
class OutputComponent;
class QImage;
class MemoryValue;

namespace colormode {
class Options;
struct ColorMode {
  using size_t = std::size_t;
  using GetPixelFunction = std::function<std::uint32_t(
      Optional<OutputComponent *>, Options &, size_t, size_t)>;
  using GetColorFunction = std::function<std::uint32_t(
      Optional<OutputComponent *>, Options &, size_t)>;
  using GetPixelFromBufferFunction = std::function<std::uint32_t(
      const MemoryValue &, size_t, Options &, size_t, size_t)>;
  using GetColorFromBufferFunction = std::function<std::uint32_t(
      const MemoryValue &, size_t, Options &, size_t)>;
  using UpdateMemoryFunction = std::function<void(Optional<OutputComponent *>,
                                                  Options &,
                                                  std::shared_ptr<QImage>,
                                                  size_t,
                                                  size_t)>;
  using UpdateAllPixelsFunction = std::function<void(
      Optional<OutputComponent *>, Options &, std::shared_ptr<QImage>)>;
  using UpdateAllColorsFunction = std::function<void(
      Optional<OutputComponent *>, Options &, std::shared_ptr<QImage>)>;

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

  /**
   * \brief returns the color of the pixel at (x,y)
   */
  GetPixelFunction getPixel;
  /**
   * \brief returns the color from the color Table at the index index
   */
  GetColorFunction getColor;
  /**
   * \brief returns the color of the pixel at (x,y) fetching the data from the
   *        prefetched Buffer buffer
   */
  GetPixelFromBufferFunction getPixelFromBuffer;
  /**
   * \brief returns the color from the color Table at the index index fetching
   *        the data from the prefetched Buffer buffer
   */
  GetColorFromBufferFunction getColorFromBuffer;
  /**
   * \brief updates the image wherever the given change in memory modified the
   *        image
   */
  UpdateMemoryFunction updateMemory;
  /**
   * \brief updates all pixels of the image
   */
  UpdateAllPixelsFunction updateAllPixels;
  /**
   * \brief updates all colors of the image
   */
  UpdateAllColorsFunction updateAllColors;

  /**
   * \brief loads a pointer from memory
   * \param memoryAccess The access to the memory
   * \param address Address of the pointer
   * \param indirect ponter is at address : pointer is address
   * \param cellSize size of a cell in memory
   * \param pointerSize size of a pointer in Bits
   * \return the pointer
   */
  static size_t loadPointer(Optional<OutputComponent *> memoryAccess,
                            size_t address,
                            bool indirect,
                            size_t cellSize,
                            size_t pointerSize,
                            Options &o);
  /**
   * \brief returns the memoryValue from memory at address
   * \param memoryAccess The access to the memory
   * \param address address of the value
   * \param length length of the value in byte
   * \param defaultLength length of the output if there is no memoryAccess
   * \return the memoryValue from memory at address
   */
  static MemoryValue getMemoryValueAt(Optional<OutputComponent *> memoryAccess,
                                      size_t address,
                                      size_t length,
                                      Options &o);
};
}

#endif  // ERAGPSIM_UI_PIXEL_DISPLAY_COLOR_MODE_HPP
