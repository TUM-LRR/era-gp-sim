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
#include "parser/final-representation.hpp"
#include "third-party/json/json.hpp"
#include "ui/editor-component.hpp"
#include "ui/input-button-model.hpp"
#include "ui/input-click-model.hpp"
#include "ui/input-text-model.hpp"
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

  /**
   * The Constructor
   *
   * \param context for the components to register themselvs
   * \param formula the architectures and extensions
   * \param memorySize the size of the memory for the memoryComponent
   * \param parserName the name of the parser
   * \param snapshotComponent A shared pointer to the snapshot component.
   * \param parent the parent, its needed for the QObject
   */
  GuiProject(QQmlContext* context,
             const ArchitectureFormula& formula,
             std::size_t memorySize,
             const std::string& parserName,
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
   * \brief Can set The global system
   * in which numbers are presented
   *
   * \param base the name of the system,
   *  for example hex, oct, bin or dec
   *
   */
  void changeSystem(std::string base);

  /**
   * \brief parses the text
   */
  void parse();

  /**
   * \brief runs all of the code
   */
  void run();

  /**
   * \brief runs the actual line
   */
  void runLine();

  /**
   * \brief runs until a breakpoint is found
   */
  void runBreakpoint();

  /**
   * \brief stops execution
   */
  void stop();

  /**
   * \brief  Resets the state of registers, memory
   * and the execution point
   */
  void reset();

  /**
   * \brief saves the project
   */
  void saveText();

  /**
   * \brief saves with another name
   *
   * \param path the path to save to.
   */
  void saveTextAs(const QUrl& path);

  /**
   * \brief Load a text file into the editor.
   *
   * \param path the path of the file.
   */
  void loadText(const QUrl& path);

  /**
   * \brief takes a snapshot
   *
   * \param qName name of the snapshot
   */
  void saveSnapshot(const QString& qName);

  /**
   * \brief Removes a snapshot.
   *
   * \param qName name of the snapshot.
   */
  Q_INVOKABLE void removeSnapshot(const QString& qName);

  /**
   * \brief loads a snapshot
   * \param qName The name of the snapshot which should be loaded.
   */
  Q_INVOKABLE void loadSnapshot(const QString& qName);

  /**
   * \returns a list of snapshot names
   *
   */
  Q_INVOKABLE QStringList getSnapshots();

  /**
   * \returns the translated help string of the command node in the specified
   * line. Returns an empty string if there is no command in that line.
   *
   * \param line line number of the command.
   */
  Q_INVOKABLE QString getCommandHelp(std::size_t line);

  /**
   * \brief Functions for converting MemoryValues to Strings.
   *  Names should explain the single Functions
   *
   * \return the string
   */
  std::function<std::string(MemoryValue)> getHexConversion();
  std::function<std::string(MemoryValue)> getBinConversion();
  std::function<std::string(MemoryValue)> getOctConversion();
  std::function<std::string(MemoryValue)> getSignedDecimalConversion();
  std::function<std::string(MemoryValue)> getUnsignedDecimalConversion();
  std::function<std::string(MemoryValue)> getDecimalFloatConversion();

  /**
   * \brief Functions for converting strings to MemoryValues.
   * Names should explain the single Functions
   *
   * \return the memoryValue
   */
  std::function<MemoryValue(std::string)> getSignedToMemoryValue();
  std::function<MemoryValue(std::string)> getHexToMemoryValue();
  std::function<MemoryValue(std::string)> getBinToMemoryValue();
  std::function<MemoryValue(std::string)> getOctToMemoryValue();
  std::function<MemoryValue(std::string)> getUnsignedToMemoryValue();
  std::function<MemoryValue(std::string)> getFloatToMemoryValue();

 private:
  /**
   * Shows a runtime error in the ui.
   *
   * \param validationResult The validation result which indicated the error.
   */
  void _throwError(const std::string& message,
                   const std::vector<std::string>& arguments);

  /**
   * \brief the module in the core
   */
  ProjectModule _projectModule;

  /**
   * \brief The model for the register
   */
  RegisterModel _registerModel;

  /**
   * \brief the model of the editor
   */
  EditorComponent _editorComponent;


  /**
   * \brief _outputComponent The model for each output item (i.e
   * lightstrip, sevensegment, console).
   */
  OutputComponent _outputComponent;

  /**
    *\brief The input-models
    */
  InputButtonModel _inputBM;
  InputTextModel _inputTM;
  InputClickModel _inputCM;

  /*
   * The c++ component for the memory.
   */
  MemoryComponentPresenter _memoryModel;

  /**
   * The default path to save the text of this project to.
   */
  QString _defaultTextFileSavePath;

  /**
   * A shared pointer to the configuration json (for snapshots,...)
   */
  std::shared_ptr<SnapshotComponent> _snapshotComponent;

  /**
   * The architecture formula, needed to save snapshot configuration.
   */
  QString _architectureFormulaString;

  /**
   * List of Final commands to access the documentation.
   */
  CommandList _commandList;

  /**
   * Map of line number to QString, caches the help text of a specific line.
   */
  LineHelpMap _helpCache;

  /**
   * \brief The Functions for the conversion
   */
  std::function<std::string(MemoryValue)> hexConversion;
  std::function<std::string(MemoryValue)> binConversion;
  std::function<std::string(MemoryValue)> octConversion;
  std::function<std::string(MemoryValue)> signedDecimalConversion;
  std::function<std::string(MemoryValue)> unsignedDecimalConversion;
  std::function<std::string(MemoryValue)> decimalFloatConversion;

  std::function<MemoryValue(std::string)> signedToMemoryValue;
  std::function<MemoryValue(std::string)> hexToMemoryValue;
  std::function<MemoryValue(std::string)> binToMemoryValue;
  std::function<MemoryValue(std::string)> octToMemoryValue;
  std::function<MemoryValue(std::string)> unsignedToMemoryValue;
  std::function<MemoryValue(std::string)> floatToMemoryValue;

 private slots:
  /**
   * updates the cached command list.
   *
   * \param finalRepresentation A final representation with a new command list.
   */
  void _updateCommandList(const FinalRepresentation& finalRepresentation);

 signals:
  /**
   * \brief A signal to notify components that a register changed.
   *
   * \param name The name of the register that changed
   */
  void registerChanged(const QString& name);

  /**
   * \brief A signal to notify components that a part of the memory changed
   *
   * \param address The address of the memory
   * \param length The number of bytes that changed
   */
  void memoryChanged(std::size_t address, std::size_t length);

  /**
   * \brief A signal to notify the gui to ask the user for a save path for a
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
};

#endif// ERAGPSIM_UI_GUIPROJECT_HPP
