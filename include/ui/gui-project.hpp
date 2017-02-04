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


#ifndef ERAGPSIM_UI_GUIPROJECT_HPP
#define ERAGPSIM_UI_GUIPROJECT_HPP

#include <QHash>
#include <QObject>
#include <QQmlContext>
#include <QString>
#include <QStringList>

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "arch/common/architecture-formula.hpp"
#include "core/memory-value.hpp"
#include "core/project-module.hpp"
#include "parser/common/final-representation.hpp"
#include "third-party/json/json.hpp"
#include "ui/console-component.hpp"
#include "ui/editor-component.hpp"
#include "ui/input-click-model.hpp"
#include "ui/input-key-model.hpp"
#include "ui/memory-component-presenter.hpp"
#include "ui/output-component.hpp"
#include "ui/register-model.hpp"
#include "ui/snapshot-component.hpp"

class QUrl;

/**
 * This Class holds the components, which will be needed
 * for the project in cpp.
 * It is only for the gui and will allow conversions.
 * It is a child of QObject, because * this allows to set a context.
 */

class GuiProject : QObject {
  Q_OBJECT

 public:
  using Json = nlohmann::json;
  using CommandList = std::vector<FinalCommand>;
  using LineHelpMap = std::unordered_map<std::size_t, QString>;
  using MemoryToStringConverter =
      std::function<std::string(const MemoryValue&)>;
  using StringToMemoryConverter =
      std::function<Optional<MemoryValue>(const std::string&, std::size_t)>;
  using MemoryToStringConverterMap = QHash<QString, MemoryToStringConverter>;
  using StringToMemoryConverterMap = QHash<QString, StringToMemoryConverter>;

  /**
   * The Constructor
   *
   * \param context QMLContext to register the components.
   * \param formula The architectures and extensions.
   * \param memorySize The size of the memory for the memoryComponent.
   * \param parserName The name of the parser.
   * \param projectName The name of the project.
   * \param snapshotComponent A shared pointer to the snapshot component.
   * \param parent The parent of this QObject.
   */
  GuiProject(QQmlContext* context,
             const ArchitectureFormula& formula,
             std::size_t memorySize,
             const std::string& parserName,
             const std::string& projectName,
             const std::shared_ptr<SnapshotComponent>& snapshotComponent,
             QObject* parent = 0);

  /**
   * Destructor
   */
  ~GuiProject();

  /**
   * Copy constructor is deleted, QObjects can not be copied.
   */
  GuiProject(const GuiProject& other) = delete;

  /**
   * Move constructor is deleted to avoid problems with the QObject members.
   */
  GuiProject(GuiProject&& other) = delete;

  /**
   * Copy assignement is deleted, QObjects can not be copied.
   */
  GuiProject& operator=(const GuiProject& other) = delete;

  /**
   * Move assignement is deleted to avoid problems with the QObject members.
   */
  GuiProject& operator=(GuiProject&& other) = delete;

  /**
   * Can set The global system
   * in which numbers are presented
   *
   * \param base the name of the system,
   *  for example hex, bin or dec
   *
   */
  void changeSystem(const std::string& base);

  /**
   * Parses the text (Forced reparse).
   */
  void parse();

  /**
   * runs all of the code
   */
  void run();

  /**
   * runs the actual line
   */
  void runLine();

  /**
   * runs until a breakpoint is found
   */
  void runBreakpoint();

  /**
   * stops execution
   */
  void stop();

  /**
   *  Resets the state of registers, memory
   * and the execution point
   */
  void reset();

  /**
   * saves the project
   */
  void saveText();

  /**
   * saves with another name
   *
   * \param path the path to save to.
   */
  void saveTextAs(const QUrl& path);

  /**
   * Load a text file into the editor.
   *
   * \param path the path of the file.
   */
  void loadText(const QUrl& path);

  /**
   * Saves a snapshot (memory and register state).
   *
   * \param qName Name of the snapshot.
   */
  void saveSnapshot(const QString& qName);

  /**
   * Saves a project file (snapshot, code and settings).
   *
   * \param url The path of the project.
   */
  void saveProject(const QUrl& url);

  /**
   * Sets the text of the editor.
   *
   * \param text The new text.
   */
  void setText(const QString& text);

  /**
   * Load a snapshot in this project. Does not reparse the code.
   *
   * \param snapshot The snapshot object.
   */
  void loadSnapshot(const Snapshot& snapshot);

  /**
   * Removes a snapshot.
   *
   * \param qName name of the snapshot.
   * \param removePermanently Whether to erase the snapshot from disk.
   */
  Q_INVOKABLE void removeSnapshot(const QString& qName, bool removePermanently);

  /**
   * loads a snapshot
   * \param qName The name of the snapshot which should be loaded.
   */
  Q_INVOKABLE void loadSnapshot(const QString& qName);

  /**
   * \returns a list of snapshot names
   *
   */
  Q_INVOKABLE QStringList getSnapshots();

  /**
   * \returns True if a snapshot with the given name already exists, else false.
   */
  Q_INVOKABLE bool snapshotExists(QString name);

  /**
   * \returns the translated help string of the command node in the specified
   * line. Returns an empty string if there is no command in that line.
   *
   * \param line line number of the command.
   */
  Q_INVOKABLE QString getCommandHelp(std::size_t line);

  /**
   * A map of function for converting MemoryValues to strings.
   *
   * \return The map.
   */
  static const MemoryToStringConverterMap& getMemoryToStringConversions();

  /**
   *  A map of functions for converting strings to MemoryValues.
   *
   * \returns The map.
   */
  static const StringToMemoryConverterMap& getStringToMemoryConversions();

 signals:
  /**
   * A signal to notify components that a register changed.
   *
   * \param name The name of the register that changed
   */
  void registerChanged(const QString& name);

  /**
   * A signal to notify components that a part of the memory changed
   *
   * \param address The address of the memory
   * \param length The number of bytes that changed
   */
  void memoryChanged(std::size_t address, std::size_t length);

  /** A signal to notify components that the size of the memory changed.
   * \param newSize The new size of the memory.
   */
  void memorySizeChanged(std::size_t newSize);

  /**
   * A signal to notify the gui to ask the user for a save path for a
   * text save.
   */
  void saveTextAs();

  /**
   * Display an error in the ui.
   *
   * \param errorMessage The error message.
   */
  void error(const QString& errorMessage);

  /**
   * Signal that the final representation of the core changed.
   * When this signal is emitted, the command list of the gui project might not
   * have been updated. Use commandListUpdated for this purpose.
   *
   * \param finalRepresentation The new FinalRepresentation.
   */
  void
  finalRepresentationChanged(const FinalRepresentation& finalRepresentation);

  /**
   * Signal that the command list of the gui project was updated.
   */
  void commandListUpdated();

  /**
   * This signal is emitted when the execution has stopped.
   */
  void executionStopped();

  /**
   * This signal is emmited when execution starts
   *
   * \param singleStep wether the button was runLine
   */
  void runClicked(bool isSingleStep);

  /**
   * Signal for ui synchronization during execution.
   */
  void guiSync();

  /**
   * Signal that the project-name changed.
   */
  void projectNameChanged(const QString& name);


 private:
  /**
   * Shows a runtime error in the ui
   * \param message The translateable that contains the message
   */
  void _throwError(const Translateable& message);

  /**
   * the module in the core
   */
  ProjectModule _projectModule;

  /**
   * The model for the register
   */
  RegisterModel _registerModel;

  /**
   * the model of the editor
   */
  EditorComponent _editorComponent;


  /**
   * _outputComponent The model for each output item (i.e
   * lightstrip, sevensegment, console).
   */
  OutputComponent _outputComponent;

  /**
    *The input-models
    */
  InputKeyModel _inputKeys;
  InputClickModel _inputMouse;

  ConsoleComponent _consoleComponent;

  /*
   * The C++ component for the memory.
   */
  MemoryComponentPresenter _memoryModel;

  /**
   * The default path to save the text of this project to.
   */
  QUrl _defaultTextFileSavePath;

  /**
   * A shared pointer to the snapshot component.
   */
  std::shared_ptr<SnapshotComponent> _snapshotComponent;

  /**
   * The architecture formula, needed to save snapshot configuration.
   */
  QString _architectureFormulaString;

  /** The parser name, saved for serialization purposes. */
  std::string _parserName;

  /** The project name, saved for serialization purposes. */
  std::string _projectName;

  /**
   * List of Final commands to access the documentation.
   */
  CommandList _commandList;

  /**
   * Map of line number to QString, caches the help text of a specific line.
   */
  LineHelpMap _helpCache;

  /** A map of conversion functions from memory values to strings. */
  static MemoryToStringConverterMap _memoryToStringMap;

  /** A map of conversion functions from strings to memory values. */
  static StringToMemoryConverterMap _stringToMemoryMap;

 private slots:
  /**
   * updates the cached command list.
   *
   * \param finalRepresentation A final representation with a new command list.
   */
  void _updateCommandList(const FinalRepresentation& finalRepresentation);

  /**
   * Slot to notify the core, part of the ui synchronization during execution.
   */
  void _notifyCore();
};

#endif  // ERAGPSIM_UI_GUIPROJECT_HPP
