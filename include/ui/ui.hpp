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
#include <tuple>
#include <vector>

#include "third-party/json/json.hpp"
#include "ui/gui-project.hpp"

class QUrl;

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
  Q_INVOKABLE void addProject(QQuickItem* tabItem,
                              QQmlComponent* projectComponent,
                              const QVariant& memorySizeQVariant,
                              const QString& architecture,
                              const QString& optionName,
                              const QString& parser);
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
  Q_INVOKABLE QStringList getOptionNames(QString architectureName) const;

  /**
   * Returns a list of parser names of an architecture.
   *
   * \param architectureName The name of the architecture.
   */
  Q_INVOKABLE QStringList getParsers(QString architectureName) const;

  /**
   * Removes a Project from the _projects vector. Does not delete it.
   *
   * \param index The index of the project to remove.
   */
  Q_INVOKABLE void removeProject(int index);

  /**
   * Calls changeSystem on the specified project.
   *
   * \param base The name of the numerical system.
   */
  Q_INVOKABLE void changeSystem(int index, QString base);

  /**
   * Calls parse on the specified project.
   *
   * \param index The index of the project.
   */
  Q_INVOKABLE void parse(int index);

  /**
   * Call run on the specified project.
   *
   * \param index The index of the project.
   */
  Q_INVOKABLE void run(int index);

  /**
   * Call runLine on the specified project.
   *
   * \param index The index of the project.
   */
  Q_INVOKABLE void runLine(int index);

  /**
   * Call runBreakpoint on the specified project.
   *
   * \param index The index of the project.
   */
  Q_INVOKABLE void runBreakpoint(int index);

  /**
   * Call stop on the specified project.
   *
   * \param index The index of the project.
   */
  Q_INVOKABLE void stop(int index);

  /**
   * Call stop on the specified project.
   *
   * \param index The index of the project.
   */
  Q_INVOKABLE void reset(int index);

  /**
   * Call saveText on the specified project.
   *
   * \param index The index of the project.
   */
  Q_INVOKABLE void saveText(int index);

  /**
   * Call saveTextAs on the specified project.
   *
   * \param index The index of the project.
   * \param name The name/path of the save.
   */
  Q_INVOKABLE void saveTextAs(int index, QUrl path);

  /**
   * Call loadText on the specified project.
   *
   * \param index The index of the project.
   * \param name The name/path of the save.
   */
  Q_INVOKABLE void loadText(int index, QUrl path);

  /**
   * Call saveSnapshot on the specified project.
   *
   * \param index The index of the project.
   * \param name The name of the snapshot.
   */
  Q_INVOKABLE void saveSnapshot(int index, QString name);

  /**
   * Call loadSnapshot on the specified project.
   *
   * \param index The index of the project.
   * \param name The name of the snapshot.
   */
  Q_INVOKABLE void loadSnapshot(int index, QString name);

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

  /** This map contains the Architectures as string and a list of their
   * extensions as vector of strings. */
  ArchitectureMap _architectureMap;

  /** The QApplication of this program. */
  QApplication _qmlApplication;

  /** The QmlEngine of this program. */
  QQmlApplicationEngine _engine;

  /** A list of pointers to the GuiProjects. */
  std::vector<GuiProject*> _projects;
};

#endif /* ERAGPSIM_UI_UI_HPP */
