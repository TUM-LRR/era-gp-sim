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

// Note: this is a QResource File (compiled into the binary, see qml.qrc)
const char* const Settings::_settingsDefaultPath =
    ":/Components/Settings/default-settings.json";

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

QVariant Settings::get(const QString& key) {
  return instance()[key];
}

Settings::Settings() : super(this, nullptr) {
}

Status Settings::load() {
  auto result = _loadJson();
  if (!result) return result.status();

  auto json = result.value();

  auto status = _checkSnapshotLocation(json);
  if (!status) return status;

  status = _checkSettingsFile(json);
  if (!status) return status;

  // We simply override all entries
  for (auto iterator = json.begin(); iterator != json.end(); ++iterator) {
    auto value = static_cast<QJsonValue>(iterator.value());
    super::insert(iterator.key(), value.toVariant());
  }

  return Status::OK;
}

QString Settings::store() {
  auto status = _store(toJson());
  return QString::fromStdString(status.message());
}

const QString& Settings::settingsDirectoryPath() const noexcept {
  return _settingsDirectoryPath;
}

const QString& Settings::settingsFilePath() const noexcept {
  return _settingsFilePath;
}

QStringList Settings::listOfAllThemeNames() const noexcept {
  if (_listOfAllThemeNames.isEmpty()) {
    QDir directory(_settingsDirectoryPath);
    directory.cd("themes");

    for (const auto& theme :
         directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      _listOfAllThemeNames.append(theme.completeBaseName());
    }

    // Swap the default theme into the first position (we guarantee this)
    auto defaultThemeIndex =
        _listOfAllThemeNames.indexOf((*this)["theme"].toString());
    assert::that(defaultThemeIndex != -1);
    _listOfAllThemeNames.swap(0, defaultThemeIndex);
  }

  return _listOfAllThemeNames;
}

Settings::Json Settings::toJson() const {
  Json json;

  for (const auto& key : super::keys()) {
    json[key] = QJsonValue::fromVariant((*this)[key]);
    assert::that(!json[key].isUndefined());
  }

  return json;
}

StatusWithValue<QString> Settings::_findSettingsDirectory() {
  static const auto path = ".erasim";
  auto directory = QDir::home();  // Represents an absolute path to $HOME.

  if (!directory.exists()) {
    return Status::Fail("Could not find home directory");
  }

  if (!directory.exists(path) && !directory.mkdir(path)) {
    return Status::Fail("Could not create settings directory");
  }

  if (!directory.cd(path)) {
    return Status::Fail("Could not find settings directory");
  }

  return directory.canonicalPath();
}

QString Settings::_findSettingsFile(const QString& directoryPath) {
  QDir directory(directoryPath);
  return directory.absoluteFilePath("settings.json");
}

StatusWithValue<Settings::Json> Settings::_loadJson() {
  auto data = _loadSettingsData();
  if (!data) return data.status();

  auto json = QJsonDocument::fromJson(data.value());

  assert::that(json.isObject());
  return json.object();
}

StatusWithValue<QByteArray> Settings::_loadSettingsData() {
  if (_settingsFilePath.isEmpty()) {
    auto status = _findSettings();
    if (!status) return status;
  }

  auto settingsLoadPath = _settingsFilePath;
  if (!QFile::exists(settingsLoadPath)) {
    settingsLoadPath = _settingsDefaultPath;
  }

  QFile file(settingsLoadPath);
  if (!file.open(QIODevice::ReadOnly)) {
    return Status::Fail("Could not open settings file for loading");
  }

  auto contents = file.readAll();

  if (contents.isEmpty()) {
    return Status::Fail("Contents of settings are empty. Why?");
  }

  return contents;
}

Status Settings::_findSettings() {
  // Ensuring atomicity with the ordering here (lookup first, assignment last).
  auto directoryResult = _findSettingsDirectory();
  if (!directoryResult) return directoryResult.status();

  // Note that it is OK if we do not find the settings file, since we can
  // use the default settings file built into the application. We *do* need
  // to find the settings directory, however. That's why we check the
  // status above.

  _settingsFilePath = _findSettingsFile(directoryResult.value());
  _settingsDirectoryPath = directoryResult.value();

  return Status::OK;
}

Status Settings::_checkSnapshotLocation(Json& json) {
  // We perform a special check for the snapshot location in case
  // it is null (the *very* first time it is run on a new machine)
  // and assign it a default path (at $HOME/.erasim/snapshots).
  assert::that(json.contains("snapshotLocation"));
  if (!json["snapshotLocation"].isNull()) {
    return Status::OK;
  }

  auto defaultLocation = QDir::home();
  defaultLocation.cd(".erasim");

  // No-Op if the directory already exists
  defaultLocation.mkdir("snapshots");
  defaultLocation.cd("snapshots");

  json["snapshotLocation"] = defaultLocation.absolutePath();

  // Store this to disk so that it will be set the next time.
  auto status = _store(json);
  if (status) return status;

  return Status::Fail(status.message() + " when defaulting snapshot location");
}

Status Settings::_checkSettingsFile(Json& json) {
  QFile file(_settingsFilePath);
  if (file.exists()) return Status::OK;

  // There is no settings file, so the default settings
  // were loaded and we have to store them to disk.
  auto status = _store(json);
  return Status::Fail(status.message() + " when creating settings file");
}

Status Settings::_store(const Json& json) {
  QJsonDocument document(json);
  QFile file(_settingsFilePath);

  // The Settings should have been loaded and the path found by now.
  assert::that(!_settingsFilePath.isEmpty());

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return Status::Fail("Could not open settings file for writing");
  }

  if (file.write(document.toJson()) == -1) {
    return Status::Fail("Could not write to settings file");
  }

  return Status::OK;
}
