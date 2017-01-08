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

#include <QDir>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVariantMap>

#include "common/assert.hpp"
#include "common/utility.hpp"
#include "ui/theme.hpp"

Theme* Theme::_theme = nullptr;

Theme* Theme::reset(const QString& themeName, QObject* parent) {
  assert::that(!themeName.isEmpty());
  delete _theme;
  return _theme = new Theme(themeName, parent);
}

Theme& Theme::instance() {
  assert::that(_theme != nullptr);
  return *_theme;
}

Theme::Theme(const QString& themeName, QObject* parent) : super(this, parent) {
  assert::that(!themeName.isEmpty());
  load(themeName);
}

void Theme::load(const QString& themeName) {
  assert::that(!themeName.isEmpty());

  const auto& json = _loadJson(themeName);

  // We simply override all entries
  for (auto iterator = json.begin(); iterator != json.end(); ++iterator) {
    const auto& value = iterator.value();
    assert::that(value.isObject());
    std::clog << "Loading " << iterator.key().toStdString() << std::endl;
    auto v = value.toObject();
    for (auto i = v.begin(); i != v.end(); ++i) {
      std::clog << i.key().toStdString() << " : "
                << i.value().toString().toStdString() << std::endl;
    }
    super::insert(iterator.key(), value.toObject().toVariantMap());
  }
}

QByteArray Theme::_loadThemeData(const QString& name) {
  auto directory =
      QDir(QString::fromStdString(Utility::rootPath()));  // QDir::home();
  assert::that(directory.exists());
  assert::that(directory.cd(".erasim/themes/" + name + ".theme"));

  QFile file(directory.filePath("theme.json"));
  assert::that(file.exists());
  assert::that(file.open(QIODevice::ReadOnly));

  auto contents = file.readAll();
  assert::that(!contents.isEmpty());

  return contents;
}

const Theme::Json& Theme::_loadJson(const QString& name) {
  auto iterator = _cache.constFind(name);
  if (iterator != _cache.cend()) {
    return iterator.value();
  }

  auto data = _loadThemeData(name);
  auto json = QJsonDocument::fromJson(data);

  assert::that(json.isObject());
  auto object = json.object();

  return _cacheTheme(name, object);
}

const Theme::Json&
Theme::_cacheTheme(const QString& name, const QJsonObject& object) {
  // Pop a "random" element (random because the container is unordered)
  if (_cache.size() == CACHE_CAPACITY) {
    _cache.erase(_cache.begin());
  }

  auto iterator = _cache.insert(name, object);
  return iterator.value();
}
