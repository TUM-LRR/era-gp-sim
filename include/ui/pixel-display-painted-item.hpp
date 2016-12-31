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
#include <cstdint>
#include <memory>

#include "common/optional.hpp"
#include "ui/pixel-display-options.hpp"

class OutputComponent;

class PixelDisplayPaintedItem : public QQuickPaintedItem {
  Q_OBJECT
  Q_PROPERTY(OutputComponent *outputComponentPointer WRITE setOutputComponent)
  using size_t = std::size_t;
  Q_PROPERTY(size_t pixelBaseAddress READ getPixelBaseAddress WRITE
                 setPixelBaseAddress)
  Q_PROPERTY(size_t colorBaseAddress READ getColorBaseAddress WRITE
                 setColorBaseAddress)
  Q_PROPERTY(size_t width READ getWidth WRITE setWidth)
  Q_PROPERTY(size_t height READ getHeight WRITE setHeight)
  Q_PROPERTY(size_t colorMode READ getColorMode WRITE setColorMode)
  Q_PROPERTY(size_t rBit READ getRBit WRITE setRBit)
  Q_PROPERTY(size_t gBit READ getGBit WRITE setGBit)
  Q_PROPERTY(size_t bBit READ getBBit WRITE setBBit)
  Q_PROPERTY(bool columns_rows READ getColumns_rows WRITE setColumns_rows)
  Q_PROPERTY(bool horizontallyMirrored READ getHorizontallyMirrored WRITE
                 setHorizontallyMirrored)
  Q_PROPERTY(bool verticallyMirrored READ getVerticallyMirrored WRITE
                 setVerticallyMirrored)
  Q_PROPERTY(bool tight READ getTight WRITE setTight)
  Q_PROPERTY(bool pixelBufferPointerLike READ getPixelBufferPointerLike WRITE
                 setPixelBufferPointerLike)
  Q_PROPERTY(bool colorTablePointerLike READ getColorTablePointerLike WRITE
                 setColorTablePointerLike)
  Q_PROPERTY(size_t freeBytes READ getFreeBytes WRITE setFreeBytes)
  Q_PROPERTY(size_t freeBits READ getFreeBits WRITE setFreeBits)

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
  Q_INVOKABLE void memoryChanged(size_t address, size_t amount);

  /*
   * \brief sets the size of the image
   * \param width new width of the image
   * \param width new height of the image
   */
  void resize(size_t width, size_t height);

  /*
   * \brief sets the OutputComponent
   * \param o pointer to the OutputComponent
   */
  void setOutputComponent(OutputComponent *o);

  /*
   * \brief sets the pixelBaseAddress
   * \param pixelBaseAddress The pixelBaseAddress to be set
   */
  void setPixelBaseAddress(size_t pixelBaseAddress);
  /*
   * \brief sets the colorBaseAddress
   * \param colorBaseAddress The colorBaseAddress to be set
   */
  void setColorBaseAddress(size_t colorBaseAddress);
  /*
   * \brief sets the width
   * \param width The width to be set
   */
  void setWidth(size_t width);
  /*
   * \brief sets the height
   * \param height The height to be set
   */
  void setHeight(size_t height);
  /*
   * \brief sets the colorMode
   * \param colorMode The colorMode to be set active
   * \Note I maybe should use enums but enums in qt are annoying
   */
  void setColorMode(size_t colorMode);
  /*
   * \brief sets the rBit
   * \param rBit The rBit to be set
   */
  void setRBit(size_t rBit);
  /*
   * \brief sets the bBit
   * \param bBit The bBit to be set
   */
  void setGBit(size_t gBit);
  /*
   * \brief sets the gBit
   * \param gBit The gBit to be set
   */
  void setBBit(size_t bBit);
  /*
   * \brief sets the columns_rows
   * \param columns_rows The columns_rows to be set
   */
  void setColumns_rows(bool columns_rows);
  /*
   * \brief sets the horizontallyMirrored
   * \param horizontallyMirrored The horizontallyMirrored to be set
   */
  void setHorizontallyMirrored(bool horizontallyMirrored);
  /*
   * \brief sets the verticallyMirrored
   * \param verticallyMirrored The verticallyMirrored to be set
   */
  void setVerticallyMirrored(bool verticallyMirrored);
  /*
   * \brief sets the tight
   * \param tight The tight to be set
   */
  void setTight(bool tight);
  /*
   * \brief sets the pixelBufferPointerLike
   * \param pixelBufferPointerLike The pixelBufferPointerLike to be set
   */
  void setPixelBufferPointerLike(bool pixelBufferPointerLike);
  /*
   * \brief sets the colorTablePointerLike
   * \param colorTablePointerLike The colorTablePointerLike to be set
   */
  void setColorTablePointerLike(bool colorTablePointerLike);
  /*
   * \brief sets the freeBytes
   * \param freeBytes The freeBytes to be set
   */
  void setFreeBytes(size_t freeBytes);
  /*
   * \brief sets the freeBits
   * \param freeBits The freeBits to be set
   */
  void setFreeBits(size_t freeBits);

  /*
   * \brief returns pixelBaseAddress
   * \returns pixelBaseAddress
   */
  size_t getPixelBaseAddress();
  /*
   * \brief returns colorBaseAddress
   * \returns colorBaseAddress
   */
  size_t getColorBaseAddress();
  /*
   * \brief returns width
   * \returns width
   */
  size_t getWidth();
  /*
   * \brief returns height
   * \returns height
   */
  size_t getHeight();
  /*
   * \brief returns ColorMode
   * \returns ColorMode
   */
  size_t getColorMode();
  /*
   * \brief returns rBit
   * \returns rBit
   */
  size_t getRBit();
  /*
   * \brief returns gBit
   * \returns gBit
   */
  size_t getGBit();
  /*
   * \brief returns bBit
   * \returns bBit
   */
  size_t getBBit();
  /*
   * \brief returns columns_rows
   * \returns columns_rows
   */
  bool getColumns_rows();
  /*
   * \brief returns horizontallyMirrored
   * \returns horizontallyMirrored
   */
  bool getHorizontallyMirrored();
  /*
   * \brief returns verticallyMirrored
   * \returns verticallyMirrored
   */
  bool getVerticallyMirrored();
  /*
   * \brief returns tight
   * \returns tight
   */
  bool getTight();
  /*
   * \brief returns pixelBufferPointerLike
   * \returns pixelBufferPointerLike
   */
  bool getPixelBufferPointerLike();
  /*
   * \brief returns colorTablePointerLike
   * \returns colorTablePointerLike
   */
  bool getColorTablePointerLike();
  /*
   * \brief returns freeBytes
   * \returns freeBytes
   */
  size_t getFreeBytes();
  /*
   * \brief returns freeBits
   * \returns freeBits
   */
  size_t getFreeBits();

 private:
  /// shared pointer to the image
  std::shared_ptr<QImage> _image;
  /// struct storing the options used to draw the image
  colorMode::Options _options;
  /// pointer to the OutputComponent used to get the memoryAccess
  Optional<OutputComponent *> _outputComponentPointer;
};

#endif// ERAGPSIM_UI_PIXEL_DISPLAY_PAINTED_ITEM_HPP
