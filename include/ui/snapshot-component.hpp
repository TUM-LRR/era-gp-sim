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
#include <QMultiHash>
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
  /*
  * We use a `QMultiHash<QString, QString>` here, which is basically a
  * `QHash<QString, QList<QString>>`, but with a nicer interface. It might seem
  * like a `QHash<QString, QSet<QString>>` would be better here, but as the
  * number of snapshots will be relatively low, the list will not cause any
  * performance issues. Futhermore, a `QList` is needed for the view anyways.
  * We have to guard the list against duplicates, as a snapshot of the same
  * architecture and value can only be saved to one file.
  */
  using SnapshotMap = QMultiHash<QString, QString>;
  using Json = Snapshot::Json;

  /**
  * Creates a string representation of the architecture formula.
  *
  * \param formula An architecture formula.
  */
  static QString architectureToString(const ArchitectureFormula& formula);

  /**
   * Constructs a SnapshotComponent, using the given path as the snapshot
   * location.
   *
   * \param path The path of the snapshot directory.
   * \param parent Pointer to the parent QObject. Defaults to 0.
   */
  explicit SnapshotComponent(const QString& snapshotDirectoryPath,
                             QObject* parent = nullptr);

  /**
   * Return a list of snapshots for a specific Architecture formula.
   *
   * \param architecture The architecture of the snapshots.
   */
  QStringList getSnapshotList(const QString& architecture);

  /**
   * Add a snapshot for a specific architecture.
   *
   * \param architectureIdentifier The architecture of the snapshots.
   * \param snapshotName The name of the snapshot to add.
   * \param data The string data of the snapshot.
   * \throws std::ios_base::failure if there is an error writing to disk.
   */
  void addSnapshot(const QString& architectureIdentifier,
                   const QString& snapshotName,
                   const std::string& data);

  /**
   * Remove a snapshot for a specific architecture.
   *
   * \param architectureIdentifier The architecture of the snapshots.
   * \param snapshotName The name of the snapshot to remove.
   * \param removePermanently Whether to erase the snapshot from disk.
   */
  void removeSnapshot(const QString& architectureIdentifier,
                      const QString& snapshotName,
                      bool removePermanently);

  /**
   * \returns The path of a snapshot of a specific architecture and name.
   *
   * \param architectureIdentifier The architecture string.
   * \param snapshotName The name of the snapshot.
   */
  std::string snapshotPath(const QString& architectureIdentifier,
                           const QString& snapshotName);

  /**
  * Imports a snapshot (copies the file into the snapshot directory)
  *
  * \param qPath The path of the snapshot file.
  */
  Q_INVOKABLE void importSnapshot(const QUrl& qPath);

  /**
   * \returns The path of the snapshot directory.
   */
  Q_INVOKABLE QString snapshotDirectory();

 public slots:

  /**
   * Assigns the snapshot directory where snapshots are to be found and stored.
   *
   * \param snapshotDirectory The new snapshot directory path.
   */
  void snapshotDirectory(const QString& snapshotDirectoryPath);

 signals:
  /** A signal that the snapshot list changed. */
  void snapshotsChanged();

  /** Signal an error during snapshot importing */
  void snapshotError(const QString& errorMessage);

 private:
  /**
   * Walks the given directory and collects all snapshots.
   * \param directory The directory at which to start walking.
   * \return A new snapshot map of all (architecture, snapshot) pairs found.
   */
  static SnapshotMap _collectSnapshots(QDir directory);

  /** The base directory of the configuration. */
  QDir _snapshotDirectory;

  /** A map of architecture-signature to a list of snapshot names. */
  SnapshotMap _snapshotMap;
};

#endif /* ERAGPSIM_UI_SNAPSHOT_COMPONENT_HPP */
