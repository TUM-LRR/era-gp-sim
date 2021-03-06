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

#ifndef ERAGPSIM_UI_UI_HPP
#define ERAGPSIM_UI_UI_HPP

#include <QApplication>
#include <QMap>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include <QString>
#include <QStringList>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include "third-party/json/json.hpp"
#include "ui/gui-project.hpp"

class QUrl;
class SnapshotComponent;
class Translateable;

/**
 * This class creates the QmlEngine and starts the qml application.
 *
 * It is also used as global context property of the qml application in order to
 * create new projects with the addProject method.
 *
 */
class Ui : public QObject {
  Q_OBJECT
 public:
  using Json = nlohmann::json;
  using ArchitectureMap =
      QMap<QString, std::tuple<QMap<QString, QStringList>, QStringList>>;
  using id_t = unsigned int;
  using size_t = std::size_t;
  using ProjectMap = std::unordered_map<id_t, GuiProject*>;

  /**
   * \brief Creates a new Ui object.
   *
   * Initializes the QmlApplication and the QmlEngine.
   *
   * \param argc argument count, passed by main function
   * \param argv argument vector, passed by main function
   */
  Ui(int& argc, char** argv);

  /**
   * Starts and runs the qml application
   *
   * This method is blocking, as it starts the message loop. It does only
   * return when the application is stopped
   *
   * \return exit code of the application
   */
  int runUi();

  /**
   *  creates a new Project, is called from qml.
   *
   * \param tabItem The QQuickItem of the tab/parent of the project in qml. This
   * is used as parent of the gui Project, therefore its lifetime is controlled
   * by the tabItem.
   * \param projectComponent The QQmlComponent to be used to create the qml part
   * of the project.
   */
  Q_INVOKABLE id_t addProject(QQuickItem* tabItem,
                              QQmlComponent* projectComponent,
                              const QVariant& memorySizeQVariant,
                              const QString& architecture,
                              const QString& optionName,
                              const QString& parser,
                              const QString& projectName);
  /**
   * Returns a list of architecture names.
   *
   */
  Q_INVOKABLE QStringList getArchitectures() const;

  /**
   * Returns a list of names of different architecture versions.
   *
   * \param architectureName The name of the architecture.
   */
  Q_INVOKABLE QStringList getOptionNames(const QString& architectureName) const;

  /**
   * Returns a list of parser names of an architecture.
   *
   * \param architectureName The name of the architecture.
   */
  Q_INVOKABLE QStringList getParsers(const QString& architectureName) const;

  /**
   * Removes a Project from the _projects map. Does not delete it.
   *
   * \param id The id of the project to remove.
   */
  Q_INVOKABLE void removeProject(id_t id);

  /**
   * Calls changeSystem on the specified project.
   *
   * \param base The name of the numerical system.
   */
  Q_INVOKABLE void changeSystem(id_t id, const QString& base);

  /**
   * Calls parse on the specified project.
   *
   * \param id The id of the project.
   */
  Q_INVOKABLE void parse(id_t id);

  /**
   * Call run on the specified project.
   *
   * \param id The id of the project.
   */
  Q_INVOKABLE void run(id_t id);

  /**
   * Call runLine on the specified project.
   *
   * \param id The id of the project.
   */
  Q_INVOKABLE void runLine(id_t id);

  /**
   * Call runBreakpoint on the specified project.
   *
   * \param id The id of the project.
   */
  Q_INVOKABLE void runBreakpoint(id_t id);

  /**
   * Call stop on the specified project.
   *
   * \param id The id of the project.
   */
  Q_INVOKABLE void stop(id_t id);

  /**
   * Call stop on the specified project.
   *
   * \param id The id of the project.
   */
  Q_INVOKABLE void reset(id_t id);

  /**
   * Call saveText on the specified project.
   *
   * \param id The id of the project.
   */
  Q_INVOKABLE void saveText(id_t id);

  /**
   * Call saveTextAs on the specified project.
   *
   * \param id The id of the project.
   * \param name The name/path of the save.
   */
  Q_INVOKABLE void saveTextAs(id_t id, const QUrl& path);

  /**
   * Call loadText on the specified project.
   *
   * \param id The id of the project.
   * \param name The name/path of the save.
   */
  Q_INVOKABLE void loadText(id_t id, const QUrl& path);

  /**
   * Call saveSnapshot on the specified project.
   *
   * \param id The id of the project.
   * \param name The name of the snapshot.
   */
  Q_INVOKABLE void saveSnapshot(id_t id, const QString& name);

  /**
   * Call loadSnapshot on the specified project.
   *
   * \param id The id of the project.
   * \param name The name of the snapshot.
   */
  Q_INVOKABLE void loadSnapshot(id_t id, const QString& name);

  /**
   * Call saveProject on the specified project.
   *
   * \param id The id of the project.
   */
  Q_INVOKABLE void saveProject(id_t id);

  /**
   * Call saveProjectAs on the specified project.
   *
   * \param id The id of the project.
   * \param path The path of the project file.
   */
  Q_INVOKABLE void saveProjectAs(id_t id, const QUrl& path);

  /**
   * Call loadProject on the specified project.
   *
   * \param path The path of the project file.
   * \param newTab The index of the tab that was created for this project.
   */
  Q_INVOKABLE id_t loadProject(QQuickItem* tabItem,
                               QQmlComponent* projectComponent,
                               const QUrl& url,
                               int newTab);


  /**
   * Translate a Translateable
   *
   * \param translateable The translateable which should be translated.
   */
  static QString translate(const Translateable& translateable);

 signals:
  /**
   * Signal to delete a tab from the tab view.
   *
   * \param errorMessage An error message for the ui.
   * \param index The index of the tab where creation failed.
   */
  void projectCreationFailed(const QString& errorMessage, int index);

 private:
  /** loads the architectures and extensions from a json file. */
  void _loadArchitectures();

  /**
   * Returns a list of extensions to create a specific architecture version.
   *
   * \param architectureName The name of the architecture.
   * \param optionName The name of the option/version.
   */
  QStringList
  _getOptionFormula(QString architectureName, QString optionName) const;

  /**
   * Registers any custom types we want QML to know about.
   */
  void _registerCustomTypes();

  /**
   * Attempts to setup the QML engine.
   * \returns True if setup was successful and the main engine can be started,
   * false if there was a configuration error and the error dialog engine was
   * already started instead.
   */
  bool _setupEngine();

  /**
   * Starts the main QML engine after loading all configuration successfully.
   */
  void _startMainEngine();

  /**
   * Starts the error QML engine when there was some error setting up the UI.
   */
  void _startErrorEngine();

  /**
   * Sets up the snapshots component.
   *
   * \param snapshotLocation The path where snapshots can be found and stored.
   */
  void _setupSnapshots(const QString& snapshotLocation);

  /**
   * Creates a new project with an own QQmlContext and a GuiProject.
   *
   * \param tabItem The QQuickItem of the tab/parent of the project in qml. This
   * is used as parent of the gui Project, therefore its lifetime is controlled
   * by the tabItem.
   * \param projectComponent The QQmlComponent to be used to create the qml part
   * of the project.
   * \param architectureFormula The architecture formula for the
   * GuiProject/core.
   * \param memorySize The memory size which is given to the core.
   * \param parserName The name of the parser which is used for this project.
   * \param projectName The name of the project.
   */
  id_t _createProject(QQuickItem* tabItem,
                      QQmlComponent* projectComponent,
                      const ArchitectureFormula& architectureFormula,
                      size_t memorySize,
                      const std::string& parserName,
                      const std::string& projectName);

  /**
  * Transelates the error message and sends a signal.
  *
  * \param message The transelateable error message.
  */
  void _sendCreationFailedSignal(const Translateable& message, int index);

  /**
   * Loads a project file.
   *
   * \param url The file path.
   * \param newTab The index of the new tab (needed to send error messages).
   * \return A snapshot generated from the project file. If any errors occur, an
   * empty snapshot wil be returned.
   */
  Snapshot _loadProjectFile(const QUrl& url, int newTab);

  /** This map contains the Architectures as string and a list of their
   * extensions as vector of strings. */
  ArchitectureMap _architectureMap;

  /** The QApplication of this program. */
  QApplication _qmlApplication;

  /** The QmlEngine of this program. */
  QQmlApplicationEngine _engine;

  /** A map of ids and pointers to the GuiProjects. */
  ProjectMap _projects;

  /** A shared pointer to a config json object. */
  std::shared_ptr<SnapshotComponent> _snapshots;

  /** An id which is incremented for each project which is created. */
  static id_t _rollingProjectId;
};

#endif /* ERAGPSIM_UI_UI_HPP */
