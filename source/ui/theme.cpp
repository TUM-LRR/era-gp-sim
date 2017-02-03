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

#include "ui/theme.hpp"

#include <QCoreApplication>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include "common/assert.hpp"
#include "common/utility.hpp"

Theme* Theme::_theme = nullptr;

Status Theme::Make(const QString& themeName) {
  assert::that(!themeName.isEmpty());
  assert::that(_theme == nullptr);

  _theme = new Theme();

  auto status = _theme->load(themeName);

  if (!status) {
    delete _theme;
    _theme = nullptr;
  }

  return status;
}

Theme& Theme::instance() {
  assert::that(_theme != nullptr);
  return *_theme;
}

Theme* Theme::pointer() {
  assert::that(_theme != nullptr);
  return _theme;
}

Theme::Theme() : super(this, nullptr) {
}

Status Theme::load(const QString& themeName) {
  assert::that(!themeName.isEmpty());

  auto result = _loadJson(themeName);
  if (!result) return result.status();

  const auto& json = result.value();

  // We simply override all entries
  for (auto iterator = json.begin(); iterator != json.end(); ++iterator) {
    const auto& value = iterator.value();
    assert::that(value.isObject());
    super::insert(iterator.key(), value.toObject().toVariantMap());
  }

  _currentThemeName = themeName;
  emit themeChanged(themeName);

  return Status::OK;
}

const QString Theme::currentThemeName() const noexcept {
  return _currentThemeName;
}

bool Theme::_copyTheme(const QString& name, QDir destination) {
  // Obtain the executable directory
  QDir directory(QCoreApplication::applicationDirPath());

  if (!directory.cd("themes/" + name + ".theme")) {
    return false;
  }

  if (!destination.mkdir(name + ".theme")) {
    return false;
  }

  if (!destination.cd(name + ".theme")) {
    return false;
  }

  // Copy theme files
  for (const auto& fileInfo : directory.entryInfoList(QDir::Files)) {
    QFile file(fileInfo.absoluteFilePath());
    if (!file.copy(destination.filePath(fileInfo.fileName()))) {
      return false;
    }
  }

  return true;
}

StatusWithValue<QByteArray> Theme::_loadThemeData(const QString& name) {
  static const auto path = ".erasim/themes";
  auto directory = QDir::home();  // Represents an absolute path to $HOME.

  if (!directory.exists()) {
    return Status::Fail("Could not find home directory");
  }

  if (!directory.exists(path) && !directory.mkdir(path)) {
    return Status::Fail("Could not create theme directory");
  }

  if (!directory.cd(path)) {
    return Status::Fail("Could not find theme directory");
  }

  if (!directory.exists(name + ".theme")) {
    // Attempt to copy the theme directory. Return value ignored, because that
    // case is handled in the subsequent if statement.
    _copyTheme(name, directory);
  }

  if (!directory.cd(name + ".theme")) {
    return Status::Fail("Could not find theme: " + name.toStdString());
  }

  QFile file(directory.filePath("theme.json"));

  if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
    return Status::Fail("Could not open theme: " + name.toStdString());
  }

  auto contents = file.readAll();

  if (contents.isEmpty()) {
    return Status::Fail("Contents of theme '" + name.toStdString() +
                        "' are corrupted");
  }

  return contents;
}

StatusWithValue<const Theme::Json&> Theme::_loadJson(const QString& name) {
  auto iterator = _cache.constFind(name);
  if (iterator != _cache.cend()) {
    return iterator.value();
  }

  auto data = _loadThemeData(name);
  if (!data) return data.status();

  auto json = QJsonDocument::fromJson(data.value());

  assert::that(json.isObject());
  auto object = json.object();

  return _cacheTheme(name, object);
}

const Theme::Json&
Theme::_cacheTheme(const QString& name, QJsonObject& object) {
  // Pop a "random" element (random because the container is unordered)
  if (_cache.size() == CACHE_CAPACITY) {
    _cache.erase(_cache.begin());
  }

  auto iterator = _cache.insert(name, std::move(object));
  return iterator.value();
}
