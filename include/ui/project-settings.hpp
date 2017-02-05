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

#ifndef ERAGPSIM_UI_PROJECT_SETTINGS_HPP
#define ERAGPSIM_UI_PROJECT_SETTINGS_HPP

#include <QQmlPropertyMap>

#include "third-party/json/json.hpp"

class QQmlContext;

/**
 * This class manages project specific settings, such as the positioning of the
 * components of the splitviews.
 */
class ProjectSettings : public QQmlPropertyMap {
  Q_OBJECT
  using super = QQmlPropertyMap;

 public:
  // QJsonObject not used here at the moment as snapshots use nlohmann::json
  using Json = nlohmann::json;


  /**
   * Constructor.
   *
   * \param context The Qml-Context of this project.
   */
  explicit ProjectSettings(QQmlContext* context);

  /**
   * Initializes this object from a json object.
   */
  void loadSettings(const Json& json);

  /**
   * Writes the settings to a json object.
   *
   * \return A json with the settings.
   */
  Json toJson();

 signals:
  /** Signal that new settings were loaded. */
  void settingsLoaded();
};

#endif /* ERAGPSIM_UI_PROJECT_SETTINGS_HPP */
