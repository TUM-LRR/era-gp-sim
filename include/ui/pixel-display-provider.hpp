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

#ifndef ERAGPSIM_UI_PIXEL_DISPLAY_HPP_
#define ERAGPSIM_UI_PIXEL_DISPLAY_HPP_

#include <QImage>
#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QQuickImageProvider>

class PixelDisplayProvider : public QQuickImageProvider {
 public:
  PixelDisplayProvider() : QQuickImageProvider(QQuickImageProvider::Image) {
  }

  QImage
  requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    int width = 100;
    int height = 100;

    if (size) *size = QSize(width, height);
    QImage image(requestedSize.width() > 0 ? requestedSize.width() : width,
                 requestedSize.height() > 0 ? requestedSize.height() : height,
                 QImage::Format_ARGB32);
    image.fill(QColor(id).rgba());
    image.setPixel(QPoint{20, 20}, 0xFF0000FFu);
    image.setPixel(QPoint{20, 80}, 0xFF0000FFu);
    image.setPixel(QPoint{80, 20}, 0xFF0000FFu);
    image.setPixel(QPoint{80, 80}, 0xFF0000FFu);

    return image;
  }
};

// class ImageProviderExtensionPlugin : public QQmlExtensionPlugin {
//   Q_OBJECT
//   Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
//  public:
//   void registerTypes(const char *uri) {
//     Q_UNUSED(uri);
//   }
//
//   void initializeEngine(QQmlEngine *engine, const char *uri) {
//     Q_UNUSED(uri);
//     engine->addImageProvider("pixeldisplayprovider", new PixelDisplayProvider);
//   }
// };

#endif// ERAGPSIM_UI_PIXEL_DISPLAY_HPP_
