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
#include "core/servant.hpp"
#include "parser/compile-error.hpp"
#include "parser/final-representation.hpp"
#include "parser/parser.hpp"


/**
 * This class holds all information about a project
 *
 * It wraps the architecture, memory and register in one object, which
 * is one servant and lives in its own thread.
 *
 * It provides memory and register access, and an interface for the ui. The ui
 * is updated with callbacks, which can be registered through this class.
 */
class Project : public Servant {
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
  Project(std::weak_ptr<Scheduler> &&scheduler,
          ArchitectureFormula &&architectureFormula,
          int memorySize);


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
  MemoryValue getRegisterValue(const std::string &name,
                               const std::size_t byteSize = 8) const;

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
  Architecture::byte_size_t getByteSize() const;

  /**
   * Returns the number of memory cells(number of bytes)
   *
   */
  int getMemorySize() const;

  /**
   * Sets the number of memory cells, might not be supported later
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
   * Reset the state of the memory
   *
   */
  void resetMemory();

  /**
   * Reset the state of the registers
   *
   */
  void resetRegisters();

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


  // set gui callbacks for registers and memory

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
   * Returns the architecture object.
   *
   */
  Architecture getArchitecture();


 private:
  /** An Architecture object, stores all information about the architecture of
   * this project. */
  Architecture _architecture;

  /** A memory object, manages the memory of this project. Not on the master
   * branch at the moment. */
  // Memory _memory;

  /** A set of registers, manages the registers of this project. */
  RegisterSet _registerSet;

  /** Callback to update a single register in the ui. */
  std::function<void(std::string)> _updateRegister;

  /** Callback to update multiple registers in the ui at the same time. */
  std::function<void(std::vector<std::string> &&)> _updateRegisters;

  /** Callback to update a memory cell in the ui. */
  std::function<void(int)> _updateMemoryCell;
};

#endif /* ERAGPSIM_CORE_PROJECT_HPP_ */
