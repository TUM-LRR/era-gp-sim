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

#ifndef ERAGPSIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP_
#define ERAGPSIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP_

#include "arch/riscv/instruction-node.hpp"

#include <string>

namespace riscv {

/**
 * \brief The LoadType enum
 *
 * The different types of a load instruction. See RISC V specification
 * for reference.
 */
enum struct LoadType {
  WORD,              // LW
  HALF_WORD,         // LH
  HALF_WORD_UNSIGNED,// LHU
  BYTE,              // LB
  BYTE_UNSIGNED      // LBU
};

/**
 * \brief The StoreType enum
 *
 * The different types of a store instruction. See RISC V specification
 * for reference.
 */
enum struct StoreType {
  WORD,     // SW
  HALF_WORD,// SH
  BYTE      // SB
};

/**
 * \brief The LoadInstructionNode class
 *
 * Represents a load instruction.
 */
class LoadInstructionNode : public InstructionNode {
 public:
  LoadInstructionNode(LoadType type) : InstructionNode(), _type(type) {
  }

  virtual MemoryValue getValue(DummyMemoryAccess &memory_access);

  virtual bool validate();

  virtual MemoryValue assemble() {
    return MemoryValue{};// TODO
  }

  virtual std::string getIdentifier() {
    switch (_type) {
      case LoadType::WORD: return "LW";
      case LoadType::HALF_WORD: return "LH";
      case LoadType::HALF_WORD_UNSIGNED: return "LHU";
      case LoadType::BYTE: return "LB";
      case LoadType::BYTE_UNSIGNED: return "LBU";
      default: return "Illegal load type";
    }
  }

 private:
  LoadType _type;
};

/**
 * \brief The StoreInstructionNode class
 *
 * Represents a store instruction.
 */
class StoreInstructionNode : public InstructionNode {
 public:
  StoreInstructionNode(StoreType type) : InstructionNode(), _type(type) {
  }

  virtual MemoryValue getValue(DummyMemoryAccess &memory_access);

  virtual bool validate();

  virtual MemoryValue assemble() {
    return MemoryValue{};// TODO
  }

  virtual std::string getIdentifier() {
    switch (_type) {
      case StoreType::WORD: return "SW";
      case StoreType::HALF_WORD: return "SH";
      case StoreType::BYTE: return "SB";
      default: return "Illegal store type";
    }
  }

 private:
  StoreType _type;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_LOAD_STORE_INSTRUCTIONS_HPP_ */
