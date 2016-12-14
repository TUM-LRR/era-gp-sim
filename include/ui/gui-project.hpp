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
#include <functional>
#include <string>

#include "arch/common/architecture-formula.hpp"
#include "core/memory-value.hpp"
#include "core/project-module.hpp"
#include "ui/editor-component.hpp"
#include "ui/memory-component-presenter.hpp"
#include "ui/output-component.hpp"
#include "ui/register-model.hpp"
//#include "ui/snapshotmodel.hpp"


/**
 * This Class holds the components, which will be needed
 * for the project in cpp.
 * It is only for the gui and will allow conversions.
 * It is a child of QObject, because * this allows to set a context.
 */

class GuiProject : QObject {
  Q_OBJECT

 public:
  /**
   * The Constructor
   *
   * \param context for the components to register themselvs
   * \param formula the architectures and extensions
   * \param memorySize the size of the memory for the memoryComponent
   * \param parserName the name of the parser
   * \param parent the parent, its needed for the QObject
   */
  GuiProject(QQmlContext* context,
             const ArchitectureFormula& formula,
             std::size_t memorySize,
             const std::string& parserName,
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
  void save();

  /**
   * \brief saves with another name
   *
   * \param name the new name
   */
  void saveAs(QString name);

  /**
   * \brief takes a snapshot
   *
   * \param name name of the snapshot
   */
  void saveSnapshot(QString name);

  /**
   * \brief loads a snapshot
   * \param name The name of the snapshot, which should be loaded
   */
  void loadSnapshot(QString name);

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

  // SnapshotModel snapmodel;
  MemoryComponentPresenter _memoryModel;
  // Core-Project;

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
};

#endif// ERAGPSIM_UI_GUIPROJECT_HPP
