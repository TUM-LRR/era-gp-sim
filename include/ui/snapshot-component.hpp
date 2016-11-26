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

#ifndef ERAGPSIM_UI_SNAPSHOT_COMPONENT_HPP
#define ERAGPSIM_UI_SNAPSHOT_COMPONENT_HPP

#include <QDir>
#include <QStringList>

#include "arch/common/architecture-formula.hpp"
#include "third-party/json/json.hpp"

class SnapshotComponent : public QObject {
  Q_OBJECT
 public:
  using Json = nlohmann::json;

  /**
   * Construct a new SnapshotComponent from a string.
   *
   * \param path The json object is loaded from this path.
   */
  SnapshotComponent(const std::string& path, QObject* parent = 0);

  /**
   * Return a list of snapshots for a specific Architecture formula.
   *
   * \param formula The architecture of the snapshots.
   */
  QStringList getSnapshotList(const QString& architecture);

  /**
   * Add a snapshot for a specific architecture.
   *
   * \param formula The architecture of the snapshots.
   * \param snapshot The name of the snapshot to add.
   */
  void addSnapshot(const QString& architecture,
                   const QString& snapshot,
                   const std::string& data);

  /**
   * Remove a snapshot for a specific architecture.
   *
   * \param formula The architecture of the snapshots.
   * \param snapshot The name of the snapshot to remove.
   */
  void removeSnapshot(const QString& architecture, const QString& snapshot);

  /**
   * Returns the path of a snapshot of a specific architecture and name.
   *
   * \param architecture The architecture string.
   * \param snapshot The name of the snapshot.
   */
  std::string
  snapshotPath(const QString& architecture, const QString& snapshot);

  /**
   * Creates a string representation of the architecture formula.
   *
   * \param formula An architecture formula.
   */
  static QString architectureToString(const ArchitectureFormula& formula);


 private:
  /** The json object is loaded and saved to this path. */
  QString _path;

  /** The base directory of the configuration. */
  QDir _baseDirectory;

  /** A map of architecture-signature to a list of extensions. */
  QHash<QString, QStringList> _snapshotMap;

 signals:
  /** A signal that the snapshot list changed. */
  void snapshotsChanged();
};

#endif /* ERAGPSIM_UI_SNAPSHOT_COMPONENT_HPP */
