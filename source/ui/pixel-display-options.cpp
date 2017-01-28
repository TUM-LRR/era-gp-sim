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

#include <QImage>

#include "common/assert.hpp"
#include "ui/color-mode.hpp"
#include "ui/pixel-display-options.hpp"

namespace colormode {

const std::string Options::errorMessages[] = {
    "Unable to access memory",
    "Pointer outside Memory",
    "The Pixel Buffer is at least partially outside Memory",
    "The Color Buffer is at least partially outside Memory",
    "A Buffers starting address is outside Memory",
};


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
                                std::size_t y) {
  return getColorMode().getPixel(memoryAccess, *this, x, y);
}
std::uint32_t
Options::getColor(Optional<OutputComponent *> memoryAccess, std::size_t index) {
  return getColorMode().getColor(memoryAccess, *this, index);
}
std::uint32_t Options::getPixelFromBuffer(const MemoryValue &buffer,
                                          std::size_t offset,
                                          std::size_t x,
                                          std::size_t y) {
  return getColorMode().getPixelFromBuffer(buffer, offset, *this, x, y);
}
std::uint32_t Options::getColorFromBuffer(const MemoryValue &buffer,
                                          std::size_t offset,
                                          std::size_t index) {
  return getColorMode().getColorFromBuffer(buffer, offset, *this, index);
}
void Options::updatePixel(Optional<OutputComponent *> memoryAccess,
                          std::shared_ptr<QImage> image,
                          std::size_t x,
                          std::size_t y) {
  image->setPixel(x, y, getPixel(memoryAccess, x, y));
}
void Options::updateColor(Optional<OutputComponent *> memoryAccess,
                          std::shared_ptr<QImage> image,
                          std::size_t index) {
  image->setColor(index, getColor(memoryAccess, index));
}
void Options::updateMemory(Optional<OutputComponent *> memoryAccess,
                           std::shared_ptr<QImage> image,
                           std::size_t address,
                           std::size_t amount) {
  getColorMode().updateMemory(memoryAccess, *this, image, address, amount);
}
void Options::updateAllPixels(Optional<OutputComponent *> memoryAccess,
                              std::shared_ptr<QImage> image) {
  getColorMode().updateAllPixels(memoryAccess, *this, image);
}
void Options::updateAllColors(Optional<OutputComponent *> memoryAccess,
                              std::shared_ptr<QImage> image) {
  getColorMode().updateAllColors(memoryAccess, *this, image);
}

std::shared_ptr<QImage> Options::makeImage() const {
  QImage::Format format = QImage::Format_RGB32;
  switch (colorMode) {
    case 0: format = QImage::Format_RGB32; break;
    case 1: format = QImage::Format_Mono; break;
    default: break;
  }
  return std::make_shared<QImage>(width, height, format);
}

void Options::setError(size_t index) {
  assert::that(index < maxError);
  assert::that(index >= 0);
  errorVector[index] = true;
}

void Options::handleErrors() {
  for (size_t i = 0; i < maxError; ++i) {
    if (errorVector[i]) {
      errorVector[i] = false;
      // TODO::Do handle
      std::cout << "PixelDisplayError " << i << " occured:" << errorMessages[i]
                << std::endl;
    }
  }
}
}
