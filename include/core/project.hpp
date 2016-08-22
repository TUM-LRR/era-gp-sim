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

#ifndef ERAGPSIM_CORE_PROJECT_HPP_
#define ERAGPSIM_CORE_PROJECT_HPP_

#include <functional>

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/instruction-set.hpp"
#include "arch/common/register-container.hpp"
#include "arch/common/register-information.hpp"
//#include "core/memory.hpp"
#include "core/memory-value.hpp"
#include "core/register-set.hpp"
#include "parser/compile-error.hpp"
#include "parser/final-representation.hpp"
#include "parser/parser.hpp"


/**
 * This class holds all information about a project
 *
 * It wraps the architecture, memory, register and parser in one object, which
 * is one servant and lives in its own thread.
 *
 * It provides memory and register access, and an interface for the ui. The ui
 * is updated with callbacks, which can be registered through this class.
 */
class Project {
 public:
  /**
   * Creates a new Project
   *
   * \param architectureFormula A formula to brew the architecture of this
   * project
   *
   * \param memorySize The number of memory cells
   *
   */
  Project(ArchitectureFormula &&architectureFormula, int memorySize);


  // memory access (WARNING: Memory is not on master, might change!)

  /**
   * Calls Memory::get(int address, int length = 1) const
   *
   */
  MemoryValue getMemory(int address, int length = 1) const;

  /**
   * Calls Memory::put(int address, const MemoryValue& value)
   *
   */
  void putMemoryCell(int address, const MemoryValue &value);

  /**
   * Calls Memory::set(int address, const MemoryValue& value)
   *
   */
  MemoryValue setMemoryCell(int address, const MemoryValue &value);

  // register access

  /**
   * Calls RegisterSet::get(const std::string& name) const
   *
   */
  MemoryValue getRegisterValue(const std::string &name) const;

  /**
   * Calls RegisterSet::get(const std::string& name, const std::size_t byteSize)
   * const
   *
   */
  MemoryValue
  getRegisterValue(const std::string &name, const std::size_t byteSize) const;

  /**
   * Calls RegisterSet::get(const std::string& name, MemoryValue&& out) const
   *
   */
  MemoryValue
  getRegisterValue(const std::string &name, MemoryValue &&out) const;

  /**
   * Calls RegisterSet::put(const std::string& name, const MemoryValue& value)
   *
   */
  void putRegisterValue(const std::string &name, const MemoryValue &value);

  /**
   * Calls RegisterSet::set(const std::string& name, const MemoryValue &value)
   *
   */
  MemoryValue
  setRegisterValue(const std::string &name, const MemoryValue &value);

  /**
   * Calls RegisterSet::set(const std::string& name, MemoryValue&& value)
   *
   */
  MemoryValue setRegisterValue(const std::string &name, MemoryValue &&value);


  // GUI interface

  // register component

  /**
   * Returns a container of all registers
   *
   */
  RegisterContainer getRegisterSet() const;

  // memory component

  /**
   * Returns the number of bits in a byte
   *
   */
  int getByteSize() const;

  /**
   * Returns the number of memory cells(number of bytes)
   *
   */
  int getMemorySize() const;

  /**
   * Sets the number of memory cells, might not be supported later TODO
   *
   */
  void setMemorySize(int size);

  // editor component

  /**
   * Returns a set of all instructions of the architecture
   *
   */
  InstructionSet getInstructionSet() const;

  /**
   * Sets a breakpoint
   *
   * \param line the line at which the breakpoint is set
   *
   */
  void setBreakpoint(int line);

  /**
   * Parses the given code
   *
   * \param code the std::string to parse
   *
   */
  void parse(std::string code);

  /**
   * Not sure if this is needed! How to decide if you call update or parse? TODO
   * updates the finalRepresentation
   *
   * \param code the updated code as std::string
   *
   */
  void update(std::string code);

  // snapshot component

  /**
   * Creates a new snapshot of all data of this project
   *
   * \param name the name of the snapshot
   *
   */
  void createSnapshot(std::string name);

  // Snapshot loadSnapshot(std::string name);

  /**
   * Deletes a snapshot
   *
   * \param name The name of the snapshot which should be deleted
   *
   */
  void deleteSnapshot(std::string name);

  // toolbar component

  /**
   * Execute the whole assembler program
   *
   */
  void execute();

  /**
   * Execute the next line of the assembler program
   *
   */
  void executeNextLine();

  /**
   * Execute the assembler program to the next breakpoint
   *
   */
  void executeToBreakpoint();

  /**
   * Stop/Pause the assembler program as soon as possible
   *
   */
  void stop();

  /**
   * Set the line which should be executed with any execute...() method
   *
   * \param line
   *
   */
  void setExecutionPoint(int line);

  /**
   * Reset the state of memory, registers and execution point
   *
   */
  void reset();

  // guiProject/QProject

  /**
   * Returns the callback used for conversion from a MemoryValue to a signed
   * decimal integer as a std::string
   *
   */
  std::function<std::string(MemoryValue)> getSignedDecimalConversion() const;

  /**
   * Returns the callback used for conversion from a MemoryValue to a unsigned
   * decimal integer as a std::string
   *
   */
  std::function<std::string(MemoryValue)> getUnsignedDecimalConversion() const;

  /**
   * Returns the callback used for conversion from a MemoryValue to a signed
   * decimal float as a std::string
   *
   */
  std::function<std::string(MemoryValue)> getFloatConversion() const;

  /**
   * Returns the callback used for conversion from a signed decimal integer as a
   * std::string to a MemoryValue
   *
   */
  std::function<MemoryValue(std::string)> getSignedToMemoryValue() const;

  /**
   * Returns the callback used for conversion from a unsigned decimal integer as
   * a std::string to a MemoryValue
   *
   */
  std::function<MemoryValue(std::string)> getUnsignedToMemoryValue() const;

  /**
   * Returns the callback used for conversion from a signed decimal float as a
   * std::string to a MemoryValue
   *
   */
  std::function<MemoryValue(std::string)> getFloatToMemoryValue() const;


  // set gui callbacks

  // register

  /**
   * Set the callback which is used to signal the gui that a register was
   * updated
   *
   * \param callback
   *
   */
  void setUpdateRegisterCallback(std::function<void(std::string)> callback);

  /**
   * Set the callback which is used to signal the gui that multiple registers
   * were updated
   *
   * \param callback
   *
   */
  void setUpdateRegistersCallback(
      std::function<void(std::vector<std::string> &&)> callback);

  // memory

  /**
   * Set the callback which is used to signal the gui that a memory cell was
   * updated
   *
   * \param callback
   *
   */
  void setUpdateMemoryCellCallback(std::function<void(int)> callback);

  /**
   * Set the callback which is used to signal the gui that context information
   * for a memory cell was changed/added
   *
   * \param callback
   *
   */
  void setSetContextInformationCallback(
      std::function<void(int, int, int, std::string)> callback);

  // editor

  /**
   * Set the callback which is used to set the error list in the gui(editor)
   *
   * \param callback
   *
   */
  void setSetErrorListCallback(
      std::function<void(std::vector<CompileError> &&)> callback);

  // void setSetMacroListCallback(std::function<void(std::vector)> callback);
  // TODO not yet know how macros are passed on by the parser

  /**
   * Set the callback which is used to inform the gui about the execution point
   * to highlight it in the editor
   *
   * \param callback
   *
   */
  void setSetCurrentLineCallback(std::function<void(int)> callback);

  // snapshots
  // TODO

  // toolbar
  // TODO


  // parser interface?


 private:
  Architecture _architecture;

  // Parser _parser;
  FinalRepresentation _representation;

  // Memory _memory; not yet on master
  RegisterSet _registerSet;


  // gui callbacks
  std::function<void(std::string)> _updateRegister;
  std::function<void(std::vector<std::string> &&)> _updateRegisters;
  std::function<void(int)> _updateMemoryCell;
  std::function<void(int, int, int, std::string)> _setContextInformation;
  std::function<void(std::vector<CompileError> &&)> _setErrorList;
  // std::function<void(std::vector)>               _setMacroList;
  std::function<void(int)> _setCurrentLine;
};

#endif /* ERAGPSIM_CORE_PROJECT_HPP_ */
