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
#include <memory>

class PixelDisplayPaintedItem : public QQuickPaintedItem {
   Q_PROPERTY(void paint)
 public:
  PixelDisplayPaintedItem(QQuickItem *parent = 0) : QQuickPaintedItem(parent) {
    _image = std::make_shared<QImage>(100, 100, QImage::Format_RGB32);
    _image->fill(QColor("#00FFFF").rgb());
  }

  void paint(QPainter *painter) {
    std::cout << "paint!" << std::endl;
    painter->drawImage(painter->window(), *_image);
  }

 private:
  std::shared_ptr<QImage> _image;
};

#endif// ERAGPSIM_UI_PIXEL_DISPLAY_PAINTED_ITEM_HPP
