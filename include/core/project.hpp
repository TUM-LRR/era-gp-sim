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
#include "arch/common/register-information.hpp"
#include "arch/common/unit-container.hpp"
#include "arch/common/unit-information.hpp"
#include "core/memory-value.hpp"
#include "core/memory.hpp"
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
          std::size_t memorySize);


  // memory access (WARNING: Memory is not on master, might change!)

  /**
   * Calls Memory::get(std::size_t address, std::size_t length = 1) const
   *
   */
  MemoryValue getMemoryValue(std::size_t address, std::size_t amount = 1);

  /**
   * Calls Memory::put(std::size_t address, const MemoryValue& value)
   *
   */
  void putMemoryValue(std::size_t address, const MemoryValue &value);

  /**
   * Calls Memory::set(std::size_t address, const MemoryValue& value)
   *
   */
  MemoryValue setMemoryValue(std::size_t address, const MemoryValue &value);

  // register access

  /**
   * Calls RegisterSet::get(const std::string& name) const
   *
   */
  MemoryValue getRegisterValue(const std::string &name) const;

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


  // GUI interface

  // register component

  /**
   * Returns a container of all registers
   *
   */
  UnitContainer getRegisterUnits() const;

  // memory component

  /**
   * Returns the number of bits in a byte
   *
   */
  // Architecture::byte_size_t getByteSize() const;

  /**
   * Returns the number of memory cells(number of bytes)
   *
   * TODO should this be supported?
   * Currently not accessible through any proxy.
   *
   */
  std::size_t getMemorySize() const;

  /**
   * Sets the number of memory cells, might not be supported later
   *
   */
  void setMemorySize(std::size_t size);

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
  void
  setUpdateRegisterCallback(std::function<void(const std::string &)> callback);

  /**
   * Set the callback which is used to signal the gui that a memory cell was
   * updated
   *
   * \param callback
   *
   */
  void setUpdateMemoryCallback(
      std::function<void(std::size_t, std::size_t)> callback);

  /**
   * Returns the architecture object.
   *
   */
  Architecture getArchitecture();


 private:
  /**
   * Creates a register.
   *
   * \param registerInfo The RegisterInformation Object of the register.
   */
  void
  createRegister(RegisterInformation registerInfo, UnitInformation unitInfo);

  /**
   * Creates all constituents of a register and recursively all constituents of
   * the constituents.
   *
   * \param enclosingRegister The register whos constituents are created
   *
   */
  void createConstituents(RegisterInformation enclosingRegister,
                          UnitInformation unitInfo);

  /** An Architecture object, stores all information about the architecture of
   * this project. */
  Architecture _architecture;

  /** A memory object, manages the memory of this project. */
  Memory _memory;

  /** A set of registers, manages the registers of this project. */
  RegisterSet _registerSet;
};

#endif /* ERAGPSIM_CORE_PROJECT_HPP_ */
