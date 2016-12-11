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
#include <QHash>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <string>

#include "core/snapshot.hpp"

class ArchitectureFormula;

/**
 * This class manages the names/paths of all snapshots.
 * Snapshots can be saved and removed through this class.
 */
class SnapshotComponent : public QObject {
  Q_OBJECT
 public:
  using SnapshotMap = QHash<QString, QSet<QString>>;
  using Json = Snapshot::Json;
  /**
   * Construct a new SnapshotComponent from a string.
   *
   * \param path The path of the snapshot directory.
   * \param parent Pointer to the parent QObject. Defaults to 0.
   */
  SnapshotComponent(const QString& path, QObject* parent = 0);

  /**
   * Return a list of snapshots for a specific Architecture formula.
   *
   * \param architecture The architecture of the snapshots.
   */
  QStringList getSnapshotList(const QString& architecture);

  /**
   * Add a snapshot for a specific architecture.
   * Can throw a std::ios_base::failure exception.
   *
   * \param architecture The architecture of the snapshots.
   * \param snapshot The name of the snapshot to add.
   * \param data The string of data of the snapshot.
   */
  void addSnapshot(const QString& architecture,
                   const QString& snapshot,
                   const std::string& data);

  /**
   * Remove a snapshot for a specific architecture.
   *
   * \param architecture The architecture of the snapshots.
   * \param snapshot The name of the snapshot to remove.
   */
  void removeSnapshot(const QString& architecture, const QString& snapshot);

  /**
   * \returns the path of a snapshot of a specific architecture and name.
   *
   * \param architecture The architecture string.
   * \param snapshot The name of the snapshot.
   */
  std::string
  snapshotPath(const QString& architecture, const QString& snapshot);

  /**
  * Imports a snapshot (copies the file into the snapshot directory)
  *
  * \param qPath The path of the snapshot file.
  */
  Q_INVOKABLE void importSnapshot(const QUrl& qPath);

  /**
  * Creates a string representation of the architecture formula.
  *
  * \param formula An architecture formula.
  */
  static QString architectureToString(const ArchitectureFormula& formula);

 private:
  /** The base directory of the configuration. */
  QDir _baseDirectory;

  /** A map of architecture-signature to a list of snapshot names. */
  SnapshotMap _snapshotMap;

  /** The file extension of snapshots. */
  static constexpr auto _fileExtension = ".snapshot";

 signals:
  /** A signal that the snapshot list changed. */
  void snapshotsChanged();

  /** Signal an error during snapshot importing */
  void snapshotError(const QString& errorMessage);
};

#endif /* ERAGPSIM_UI_SNAPSHOT_COMPONENT_HPP */
