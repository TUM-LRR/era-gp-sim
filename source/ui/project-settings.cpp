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

#include "ui/project-settings.hpp"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QQmlContext>

ProjectSettings::ProjectSettings(QQmlContext* context) : super(this, nullptr) {
  context->setContextProperty("projectSettings", this);
}

void ProjectSettings::loadSettings(const Json& json) {
  // Convert to a QJsonDocument, as this makes it easy to convert the values to
  // QVariants. Snapshots should probably be refactored to use QJsonDocument at
  // some point.
  auto data = QByteArray::fromStdString(json.dump(4));
  auto qJson = QJsonDocument::fromJson(data).object();

  // insert all entries into the map
  for (auto iterator = qJson.begin(); iterator != qJson.end(); ++iterator) {
    auto value = static_cast<QJsonValue>(iterator.value());
    super::insert(iterator.key(), value.toVariant());
  }
  emit settingsLoaded();
}

ProjectSettings::Json ProjectSettings::toJson() {
  QJsonObject json;
  // insert all entries into a json object.
  for (const auto& key : super::keys()) {
    auto value = QJsonValue::fromVariant((*this)[key]);
    json[key] = value;
  }
  auto data = QJsonDocument(json).toJson();
  return Json::parse(data.toStdString());
}
