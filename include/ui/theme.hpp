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

#ifndef ERAGPSIM_UI_THEME_HPP
#define ERAGPSIM_UI_THEME_HPP

#include <QByteArray>
#include <QDir>
#include <QHash>
#include <QJsonObject>
#include <QQmlPropertyMap>
#include <QString>

#include <cstddef>

#include "common/status-with-value.hpp"
#include "common/status.hpp"

/**
 * Represents the theme being used to style the GUI.
 *
 * Essentially, a theme is nothing else than a large key-value store, mapping
 * UI entities (such as buttons or windows) to colors, sizes and other stylistic
 * properties. As this class inherits from `QQmlPropertyMap`, it's fields can
 * be used directly in QML.
 *
 * The only function of a theme object, right now, is to load a theme from
 * a configuration-defined locatio.
 *
 * This class is designed as a singleton object and should be referenced via
 * its static `instance()` method.  It is the user's responsbility to ensure
 * that it was first initialized via `reset()`. This is required because the
 * object must be allocated as a pointer, since the QML engine will take
 * ownership of the object.
 */
class Theme : public QQmlPropertyMap {
  Q_OBJECT
  Q_PROPERTY(QString current READ currentThemeName NOTIFY themeChanged);

  using super = QQmlPropertyMap;

 public:
  /**
   * Creates a new Theme instance.
   *
   * This factory function should only be called once.
   * The class does not take ownership of the allocated theme, it
   * only provides access to it.
   *
   * \param themeName The theme to initialize the new theme singleton with.
   * \returns A pointer to the newly created theme.
   */
  static Status Make(const QString& themeName);

  /**
   * Note: Make() must have been called before.
   * \returns A reference to the theme instance.
   * \see Make()
   * \see pointer()
   */
  static Theme& instance();

  /**
   * Note: Make() must have been called before.
   * \returns A pointer to the theme instance.
   * \see Make()
   * \see instance()
   */
  static Theme* pointer();

  /**
   * Loads the theme with the given name from disk.
   *
   * Internally cached.
   *
   * \param themeName The theme to load.
   */
  Q_INVOKABLE Status load(const QString& themeName);

  /**
   * \returns The name of the theme currently loaded into the singleton.
   */
  Q_INVOKABLE const QString currentThemeName() const noexcept;

 signals:

  /**
   * A signal sent when the current theme loaded into the singleton changes.
   * \paran newName The name of the newly loaded theme.
   */
  void themeChanged(const QString& newName);

 private:
  using Json = QJsonObject;

  /**
   * How many themes to cache before evicting a random one.
   */
  static const std::size_t CACHE_CAPACITY = 8;

  /** The singleton. */
  static Theme* _theme;

  /**
   * Attempts to copy a theme from the resource directory (located in the
   * theme/ directory relative to the executable/working directory) into the
   * given destination directory.
   *
   * \param name The name of the theme to copy.
   * \param destination the destination directory, to copy the theme into.
   * \return true if theme could be copied into the destination directory.
   */
  static bool _copyTheme(const QString& name, QDir destination);

  /**
   * Loads raw data from disk for a given theme.
   *
   * \param name The name of the theme to load.
   * \returns A QByteArray of raw JSON bytes.
   */
  static StatusWithValue<QByteArray> _loadThemeData(const QString& name);

  /**
   * Constructor.
   */
  Theme();

  /**
   * Loads the JSON for a theme.
   *
   * The theme's JSON will either be retrieved
   * from the cache or loaded from disk.
   *
   * \param name The name of the theme to load.
   * \return The JSON of the specified theme.
   */
  StatusWithValue<const Json&> _loadJson(const QString& name);

  /**
   * Stores a theme's JSON in the cache.
   *
   * If the cache's capacity is reached, a random existing theme will be evicted
   * in favor of the new theme. This is the simplest replacement strategy we can
   * use without additional data structures and optimal lookup complexity.
   *
   * \param  name The name of the theme to load.
   * \param  object The json object to store in the cache.
   * \return A reference to the cached object. The old object is invalidated!
   */
  const Json& _cacheTheme(const QString& name, Json& object);

  /** A cache to store recently-loaded styles. */
  QHash<QString, Json> _cache;

  /** The name of the theme currently loaded into the singleton. */
  QString _currentThemeName;
};

#endif /* ERAGPSIM_UI_THEME_HPP */
