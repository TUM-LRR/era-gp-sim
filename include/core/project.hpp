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
#include <string>
#include <vector>

#include "arch/common/architecture-formula.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/instruction-set.hpp"
#include "arch/common/unit-container.hpp"
#include "core/memory-value.hpp"
#include "core/memory.hpp"
#include "core/register-set.hpp"
#include "core/servant.hpp"
#include "core/snapshot.hpp"

class RegisterInformation;
class UnitInformation;

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
  template <typename... T>
  using Callback = std::function<void(T...)>;
  using ErrorCallback =
      Callback<const std::string &, const std::vector<std::string> &>;

  using size_t = std::size_t;
  using MemoryValueToString = std::function<std::string(MemoryValue)>;
  using StringToMemoryValue = std::function<MemoryValue(std::string)>;

  using Json = Snapshot::Json;

  /**
   * Creates a new Project
   *
   * \scheduler A scheduler to create this active object.
   *
   * \param architectureFormula A formula to brew the architecture of this
   * project
   *
   * \param memorySize The number of memory cells
   *
   */
  Project(std::weak_ptr<Scheduler> &&scheduler,
          const ArchitectureFormula &architectureFormula,
          size_t memorySize);

  /**
   * \copydoc Memory::get()
   */
  MemoryValue getMemoryValueAt(size_t address, size_t amount = 1) const;

  /**
   * \copydoc Memory::tryGet()
   */
  MemoryValue tryGetMemoryValueAt(size_t address, size_t amount = 1) const;


  /**
   * \copydoc Memory::put()
   */
  void putMemoryValueAt(size_t address,
                        const MemoryValue &value,
                        bool ignoreProtection = false);

  /**
   * \copydoc Memory::tryPut()
   */
  void tryPutMemoryValueAt(size_t address,
                           const MemoryValue &value,
                           bool ignoreProtection = false);

  /**
   *  \copydoc Memory::set()
   */
  MemoryValue setMemoryValueAt(size_t address,
                               const MemoryValue &value,
                               bool ignoreProtection = false);

  /**
   * \copydoc Memory::trySet()
   */
  MemoryValue trySetMemoryValueAt(size_t address,
                                  const MemoryValue &value,
                                  bool ignoreProtection = false);

  /**
   * \copydoc RegisterSet::get()
   */
  MemoryValue getRegisterValue(const std::string &name) const;

  /**
   * \copydoc RegisterSet::put()
   */
  void putRegisterValue(const std::string &name, const MemoryValue &value);

  /**
   * \copydoc RegisterSet::set()
   */
  MemoryValue
  setRegisterValue(const std::string &name, const MemoryValue &value);

  /**
   * Returns a container of all registers
   *
   */
  UnitContainer getRegisterUnits() const;

  /**
   * Returns the number of bits in a byte
   *
   */
  Architecture::byte_size_t getByteSize() const;

  /**
   * Returns the number of memory cells(number of bytes)
   *
   * Currently not accessible through any proxy.
   *
   */
  size_t getMemorySize() const;

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

  /**
   * Loads a snapshot object and sets memory and registers accordingly.
   *
   * \param snapshotData The json snapshot object.
   */
  void loadSnapshot(const Json &snapshotData);

  /**
   * Generates a snapshot of the current state of memory and registers.
   *
   * \return The generated json object.
   */
  Json generateSnapshot() const;

  /**
   * Returns the callback used for conversion from a MemoryValue to a signed
   * decimal integer as a std::string
   *
   */
  MemoryValueToString getSignedDecimalConversion() const;

  /**
   * Returns the callback used for conversion from a MemoryValue to a unsigned
   * decimal integer as a std::string
   *
   */
  MemoryValueToString getUnsignedDecimalConversion() const;

  /**
   * Returns the callback used for conversion from a MemoryValue to a signed
   * decimal float as a std::string
   *
   */
  MemoryValueToString getFloatConversion() const;

  /**
   * Returns the callback used for conversion from a signed decimal integer as a
   * std::string to a MemoryValue
   *
   */
  StringToMemoryValue getSignedToMemoryValue() const;

  /**
   * Returns the callback used for conversion from a unsigned decimal integer as
   * a std::string to a MemoryValue
   *
   */
  StringToMemoryValue getUnsignedToMemoryValue() const;

  /**
   * Returns the callback used for conversion from a signed decimal float as a
   * std::string to a MemoryValue
   *
   */
  StringToMemoryValue getFloatToMemoryValue() const;

  /**
   * Set the callback which is used to signal the gui that a register was
   * updated
   *
   * \param callback
   *
   */
  void setUpdateRegisterCallback(Callback<const std::string &> callback);

  /**
   * Set the callback which is used to signal the gui that a memory cell was
   * updated
   *
   * \param callback
   *
   */
  void setUpdateMemoryCallback(Callback<size_t, size_t> callback);

  /**
   * Set the callback which is used to notify the gui of an error.
   *
   * \param callback
   */
  void setErrorCallback(ErrorCallback callback);

  /**
   * Returns the architecture object.
   *
   */
  Architecture getArchitecture() const;

 private:
  /**
   * Creates a register.
   *
   * \param registerInfo The RegisterInformation Object of the register.
   */
  void
  _createRegister(RegisterInformation registerInfo, UnitInformation unitInfo);

  /**
   * Creates all constituents of a register and recursively all constituents of
   * the constituents.
   *
   * \param enclosingRegister The register whos constituents are created
   *
   */
  void _createConstituents(RegisterInformation enclosingRegister,
                           UnitInformation unitInfo);

  /**
   * Sets the specified register to 0, if it is a top level register (no
   * constituent) and not constant.
   *
   * \param registerInfo the register information object
   */
  void _setRegisterToZero(RegisterInformation registerInfo);

  /** An Architecture object, stores all information about the architecture of
   * this project. */
  Architecture _architecture;

  /** A memory object, manages the memory of this project. */
  Memory _memory;

  /** A set of registers, manages the registers of this project. */
  RegisterSet _registerSet;

  /** Stores the architecture formula for serialization purposes. */
  ArchitectureFormula _architectureFormula;

  /** A callback to signal a error to the ui. */
  ErrorCallback _errorCallback;
};

#endif /* ERAGPSIM_CORE_PROJECT_HPP_ */
