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

#ifndef ERAGPSIM_UI_SETTINGS_HPP
#define ERAGPSIM_UI_SETTINGS_HPP

#include <QByteArray>
#include <QJsonObject>
#include <QQmlPropertyMap>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "common/status-with-value.hpp"
#include "common/status.hpp"

class Settings : public QQmlPropertyMap {
  Q_OBJECT
  using super = QQmlPropertyMap;

 public:
  using Json = QJsonObject;

  /**
   * Creates a new Settings instance.
   *
   * This factory function should only be called once.
   * The class does not take ownership of the allocated settings, it
   * only provides access to it.
   *
   * \returns A pointer to the newly created theme.
   */
  static Status Make();

  /**
   * Note: Make() must have been called before.
   * \returns A reference to the theme instance.
   * \see Make()
   * \see pointer()
   */
  static Settings& instance();

  /**
   * Note: Make() must have been called before.
   * \returns A pointer to the theme instance.
   * \see Make()
   * \see instance()
   */
  static Settings* pointer();

  /**
   * Utility method to lookup a value in the setting singleton.
   *
   * `Settings::get(key)` is equivalent to `Settings::instance()[key]`.
   *
   * \param key The key to look for.
   * \return The value for the key.
   */
  static QVariant get(const QString& key);

  /**
   * Loads the settings from disk into the `Settings` object.
   * \returns A status object indicating the success of the operation.
   */
  Status load();

  /**
   * Stores the current settings to disk.
   * \returns A message string if an error occurs, else the empty string.
   */
  Q_INVOKABLE QString store();

  /**
   * \returns The directory path of the settings.
   * \details This is the directory where themes, settings.json etc. can be
   * found.
   */
  Q_INVOKABLE const QString& settingsDirectoryPath() const noexcept;

  /**
   * \returns The file path of the settings file (settings.json).
   */
  Q_INVOKABLE const QString& settingsFilePath() const noexcept;

  /**
   * \returns A list of all theme names available with the default
   * theme (that is stored in the settings) in the first position in the list.
   */
  Q_INVOKABLE QStringList listOfAllThemeNames() const noexcept;

  /**
   * \returns A JSON object containing the contents of the settings.
   */
  Json toJson() const;

 signals:

  /**
   * Signal sent when the `snapshotLocation` property changes.
   * \param newPath The new path where snapshots are to be found and stored.
   */
  Q_INVOKABLE void snapshotLocationChanged(const QString& newPath) const;

 private:
  /** The singleton. */
  static Settings* _settings;

  /** The path to the default settings. */
  static const char* const _settingsDefaultPath;

  /**
   * Attempts to locate the settings directory at the expected path.
   *
   * This is the .erasim directory's canonical path.
   *
   * \returns A status, possibly with the directory path as the value.
   */
  static StatusWithValue<QString> _findSettingsDirectory();

  /**
   * Attempts to locate the settings file from the settings directory path.
   *
   * Should be .erasim/settings.json (in canonical format).
   *
   * \param directoryPath The path of the settings directory.
   * \returns The file path of the settings. The file may not exist.
   */
  static QString _findSettingsFile(const QString& directoryPath);

  /**
   * Constructor.
   */
  Settings();

  /**
   * Loads the JSON for the settings.
   *
   * \return The JSON of the specified theme.
   */
  StatusWithValue<Json> _loadJson();

  /**
   * Loads raw settings data from disk.
   *
   * \returns A QByteArray of raw JSON bytes.
   */
  StatusWithValue<QByteArray> _loadSettingsData();

  /**
   * Finds the settings directory and file.
   *
   * If found, sets the `_settingsFilePath` and `_settingsDirectoryPath`.
   * \returns A status for finding the paths.
   */
  Status _findSettings();

  /**
   * Checks if the snapshot location is null in the JSON and coalesces it
   * otherwise.
   *
   * \param json The newly loaded JSON object.
   * \returns A status object indicating the success of the operation.
   */
  Status _checkSnapshotLocation(Json& json);

  /**
   * Checks if the settings file exists and creates it otherwise from a settings
   * JSON.
   *
   * \param json The newly loaded JSON object.
   * \returns A status object indicating the success of the operation.
   */
  Status _checkSettingsFile(Json& json);

  /**
   * Stores the given JSON object to disk.
   *
   * \param json The JSON object to store.
   * \returns A status object indicating the success of the operation.
   */
  Status _store(const Json& json);

  /** The lazily determined settings file path. */
  QString _settingsFilePath;

  /** The lazily determined settings directory path. */
  QString _settingsDirectoryPath;

  /** A list of all theme names. */
  mutable QStringList _listOfAllThemeNames;
};


#endif /* ERAGPSIM_UI_SETTINGS_HPP */
