/*
* ERASIM Assembler Interpreter
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

#include "ui/snapshot-component.hpp"

#include <QFileInfo>
#include <QUrl>
#include <string>

#include "arch/common/architecture-formula.hpp"
#include "common/utility.hpp"
#include "core/snapshot.hpp"

QString
SnapshotComponent::architectureToString(const ArchitectureFormula& formula) {
  auto identifier = formula.getArchitectureName();
  for (const auto& extension : Utility::sorted(formula)) {
    identifier += extension;
  }
  return QString::fromStdString(identifier);
}

SnapshotComponent::SnapshotComponent(const QString& snapshotDirectoryPath,
                                     QObject* parent)
: QObject(parent) {
  snapshotDirectory(snapshotDirectoryPath);
}

QStringList SnapshotComponent::getSnapshotList(const QString& architecture) {
  return _snapshotMap.values(architecture);
}

void SnapshotComponent::addSnapshot(const QString& architectureIdentifier,
                                    const QString& snapshotName,
                                    const std::string& data) {
  _snapshotDirectory.mkdir(architectureIdentifier);

  auto path = snapshotPath(architectureIdentifier, snapshotName);
  Utility::storeToFile(path, data);

  if (!snapshotExists(architectureIdentifier, snapshotName)) {
    // As the _snapshotMap is a QMultiHash, duplicates of key value pairs can be
    // inserted, which is stopped by this.
    _snapshotMap.insert(architectureIdentifier, snapshotName);
    emit snapshotsChanged();
  }
}

void SnapshotComponent::removeSnapshot(const QString& architectureIdentifier,
                                       const QString& snapshotName,
                                       bool removePermanently) {
  if (removePermanently) {
    auto path = snapshotPath(architectureIdentifier, snapshotName);
    QFile(QString::fromStdString(path)).remove();
  }

  _snapshotMap.remove(architectureIdentifier, snapshotName);
  emit snapshotsChanged();
}

bool SnapshotComponent::snapshotExists(const QString& architectureIdentifier,
                                       const QString& snapshotName) {
  return _snapshotMap.contains(architectureIdentifier, snapshotName);
}

std::string
SnapshotComponent::snapshotPath(const QString& architectureIdentifier,
                                const QString& snapshotName) {
  auto path = _snapshotDirectory;
  path.cd(architectureIdentifier);
  auto fileName = snapshotName + ".snapshot";
  return path.absoluteFilePath(fileName).toStdString();
}

void SnapshotComponent::importSnapshot(const QUrl& qPath) {
  auto path = qPath.path().toStdString();
  try {
    auto json = Json::parse(Utility::loadFromFile(path));
    Snapshot snapshot(json);
    if (!snapshot.isValid()) {
      emit snapshotError("Import failed: Snapshot not valid.");
      return;
    }
    auto architectureFormula = snapshot.getArchitectureFormula();
    auto architectureString = architectureToString(architectureFormula);
    QFileInfo fileInfo(qPath.path());
    addSnapshot(architectureString, fileInfo.completeBaseName(), json.dump(4));
  } catch (const std::exception& exception) {
    emit snapshotError("Import failed: " +
                       QString::fromStdString(exception.what()));
  }
}

QString SnapshotComponent::snapshotDirectory() {
  return _snapshotDirectory.absolutePath();
}

void SnapshotComponent::snapshotDirectory(
    const QString& snapshotDirectoryPath) {
  assert::that(!snapshotDirectoryPath.isEmpty());
  // Creates the directory if it does not exist yet.
  _snapshotDirectory.setPath(snapshotDirectoryPath);
  QDir::root().mkpath(snapshotDirectory());
  _snapshotMap = _collectSnapshots(_snapshotDirectory);
  emit snapshotsChanged();
}

SnapshotComponent::SnapshotMap
SnapshotComponent::_collectSnapshots(QDir directory) {
  SnapshotMap snapshots;

  for (const auto& architectureIdentifier :
       directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
    directory.cd(architectureIdentifier);

    // Get all snapshots names in the directory.
    for (const auto& file : directory.entryInfoList({"*.snapshot"})) {
      snapshots.insert(architectureIdentifier, file.completeBaseName());
    }

    // Go back to the snapshot root
    directory.cdUp();
  }

  return snapshots;
}
