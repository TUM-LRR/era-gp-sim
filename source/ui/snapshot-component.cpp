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

#include "ui/snapshot-component.hpp"

#include <QFileInfo>
#include <QUrl>
#include <algorithm>

#include "arch/common/architecture-formula.hpp"
#include "common/utility.hpp"
#include "core/snapshot.hpp"

SnapshotComponent::SnapshotComponent(const std::string& path, QObject* parent)
: QObject(parent), _baseDirectory(QString::fromStdString(path)) {
  // check if snapshot directory exists, if not, create it.
  if (!_baseDirectory.exists()) {
    QString baseDirName = _baseDirectory.dirName();
    _baseDirectory.cdUp();
    _baseDirectory.mkpath(QString::fromStdString(path));
    _baseDirectory.cd(baseDirName);
  }
  QString fileExtensionFilter(_fileExtension);
  fileExtensionFilter.prepend("*");
  QStringList filterList = {fileExtensionFilter};
  // Find all subdirectories and their snapshots
  auto subDirectories =
      _baseDirectory.entryList(QDir::AllDirs | QDir::NoDot | QDir::NoDotDot);
  for (const auto& subDirectoryName : subDirectories) {
    // There is a subdirectory for every combination of architecutre and
    // extensions.
    QDir subDirectory = _baseDirectory;
    if (subDirectory.cd(subDirectoryName)) {
      // Get all snapshots names in the directory.
      auto files = subDirectory.entryInfoList(
          filterList, QDir::Files | QDir::NoDot | QDir::NoDotDot);
      QSet<QString> snapshots;
      for (const auto& file : files) {
        snapshots.insert(file.completeBaseName());
      }
      _snapshotMap.insert(subDirectory.dirName(), snapshots);
    }
  }
}

QStringList SnapshotComponent::getSnapshotList(const QString& architecture) {
  return _snapshotMap.value(architecture).toList();
}

void SnapshotComponent::addSnapshot(const QString& architecture,
                                    const QString& snapshot,
                                    const std::string& data) {
  if (!_baseDirectory.exists(architecture)) {
    _baseDirectory.mkpath(architecture);
  }
  std::string path = snapshotPath(architecture, snapshot);
  Utility::storeToFile(path, data);
  _snapshotMap[architecture].insert(snapshot);
  emit snapshotsChanged();
}

void SnapshotComponent::removeSnapshot(const QString& architecture,
                                       const QString& snapshot) {
  _snapshotMap[architecture].remove(snapshot);
  std::string path = snapshotPath(architecture, snapshot);
  std::remove(path.c_str());
  emit snapshotsChanged();
}

std::string SnapshotComponent::snapshotPath(const QString& architecture,
                                            const QString& snapshot) {
  return Utility::joinPaths(_baseDirectory.absolutePath().toStdString(),
                            architecture.toStdString(),
                            snapshot.toStdString() + _fileExtension);
}

void SnapshotComponent::importSnapshot(QUrl qPath) {
  std::string path = qPath.path().toStdString();
  try {
    Json json = Json::parse(Utility::loadFromFile(path));
    Snapshot snapshot(json);
    if (!snapshot.isValid()) {
      emit snapshotError("Import failed: Snapshot not valid.");
      return;
    }
    ArchitectureFormula architectureFormula = snapshot.getArchitectureFormula();
    QString architectureString = architectureToString(architectureFormula);
    QFileInfo fileInfo(qPath.path());
    addSnapshot(architectureString, fileInfo.completeBaseName(), json.dump(4));
  } catch (const std::exception& exception) {
    emit snapshotError("Import failed: " +
                       QString::fromStdString(exception.what()));
  }
}

QString
SnapshotComponent::architectureToString(const ArchitectureFormula& formula) {
  std::vector<std::string> underlying = formula.getUnderlying();
  std::sort(underlying.begin(), underlying.end());
  std::string architectureFormulaString = formula.getArchitectureName();
  for (const std::string& s : formula) {
    architectureFormulaString += s;
  }
  return QString::fromStdString(architectureFormulaString);
}
