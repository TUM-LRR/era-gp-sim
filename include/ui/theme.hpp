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

  static const std::size_t CACHE_CAPACITY = 8;

  /** The singleton. */
  static Theme* _theme;

  static QByteArray _loadThemeData(const QString& name);

  /**
   * (Private) constructor.
   * \param  themeName The name of the theme to construct with.
   * \param  parent    Optionally, a parent object for this QObject.
   */
  explicit Theme(const QString& themeName, QObject* parent = nullptr);

  const Json& _loadJson(const QString& name);

  const Json& _cacheTheme(const QString& name, const Json& object);

  /**
   * A cache to store recently-loaded styles.
   */
  QHash<QString, Json> _cache;
};

#endif /* ERAGPSIM_UI_THEME_HPP */
