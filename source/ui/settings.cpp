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

#include "ui/settings.hpp"

#include <QDir>
#include <QFileInfo>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVariantMap>

#include "common/assert.hpp"
#include "common/utility.hpp"

Settings* Settings::_settings = nullptr;

Status Settings::Make() {
  assert::that(_settings == nullptr);

  _settings = new Settings();

  auto status = _settings->load();

  if (!status) {
    delete _settings;
    _settings = nullptr;
  }

  return status;
}

Settings& Settings::instance() {
  assert::that(_settings != nullptr);
  return *_settings;
}

Settings* Settings::pointer() {
  assert::that(_settings != nullptr);
  return _settings;
}

Status Settings::load() {
  auto result = _loadJson();
  if (!result) return result.status();

  const auto& json = result.value();

  // We simply override all entries
  for (auto iterator = json.begin(); iterator != json.end(); ++iterator) {
    const auto& value = iterator.value();
    super::insert(iterator.key(), value.toVariant());
  }

  return Status::OK;
}

const QString& Settings::settingsDirectoryPath() const noexcept {
  return _settingsDirectoryPath;
}

const QString& Settings::settingsFilePath() const noexcept {
  return _settingsFilePath;
}

StatusWithValue<QString> Settings::_findSettingsDirectory() {
  auto directory =
      QDir(QString::fromStdString(Utility::rootPath()));  // QDir::home();

  if (!directory.exists()) {
    return Status(Status::FAILURE, "Could not find home directory");
  }

  if (!directory.cd(".erasim")) {
    return Status(Status::FAILURE, "Could not find settings directory");
  }

  return directory.canonicalPath();
}

StatusWithValue<QString>
Settings::_findSettingsFile(const QString& directoryPath) {
  QDir directory(directoryPath);
  QFileInfo file(directory.filePath("settings.json"));

  if (!file.exists()) {
    return Status(Status::FAILURE, "Could not find settings file");
  }

  return file.canonicalFilePath();
}

StatusWithValue<QByteArray> Settings::_loadSettingsData() {
  if (_settingsFilePath.isEmpty()) {
    auto status = _findSettings();
    if (!status) return status;
  }

  QFile file(_settingsFilePath);
  if (!file.open(QIODevice::ReadOnly)) {
    return Status(Status::FAILURE, "Could not open settings file for loading");
  }

  auto contents = file.readAll();

  if (contents.isEmpty()) {
    return Status(Status::FAILURE, "Contents of settings are empty. Why?");
  }

  return contents;
}

Status Settings::_findSettings() {
  // Ensuring atomicity with the ordering here (lookup first, assignment last).
  auto directoryResult = _findSettingsDirectory();
  if (!directoryResult) return directoryResult.status();

  auto fileResult = _findSettingsFile(directoryResult.value());
  if (!fileResult) return fileResult.status();

  _settingsDirectoryPath = directoryResult.value();
  _settingsFilePath = fileResult.value();

  return Status::OK;
}

StatusWithValue<Settings::Json> Settings::_loadJson() {
  auto data = _loadSettingsData();
  if (!data) return data.status();

  auto json = QJsonDocument::fromJson(data.value());

  assert::that(json.isObject());
  return json.object();
}
