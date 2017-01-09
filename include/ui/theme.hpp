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
#include <QFile>
#include <QHash>
#include <QJsonObject>
#include <QQmlPropertyMap>
#include <QString>

#include <cstddef>

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
  using super = QQmlPropertyMap;

 public:
  /**
   * Resets the theme singleton.
   * \param  themeName The theme to initialize the new theme singleton with.
   * \param  parent    Optionally, a parent object for this QObject.
   * \returns A pointer to the newly created theme.
   */
  static Theme* reset(const QString& themeName, QObject* parent = nullptr);

  /** \returns A reference to the theme instance. */
  static Theme& instance();

  /**
   * Loads the theme with the given name from disk.
   *
   * Internally cached.
   *
   * \param themeName The theme to load.
   */
  void load(const QString& themeName);

 private:
  using Json = QJsonObject;

  /**
   * How many themes to cache before evicting a random one.
   */
  static const std::size_t CACHE_CAPACITY = 8;

  /** The singleton. */
  static Theme* _theme;

  static QByteArray _loadThemeData(const QString& name);

  /**
   * (Private) constructor.
   * \param themeName The name of the theme to construct with.
   * \param parent Optionally, a parent object for this QObject.
   */
  explicit Theme(const QString& themeName, QObject* parent = nullptr);

  /**
   * Loads the JSON for a theme.
   *
   * The theme's JSON will either be retrieved
   * from the cache or loaded from disk.
   *
   * \param name The name of the theme to load.
   * \return The JSON of the specified theme.
   */
  const Json& _loadJson(const QString& name);

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
};

#endif /* ERAGPSIM_UI_THEME_HPP */
