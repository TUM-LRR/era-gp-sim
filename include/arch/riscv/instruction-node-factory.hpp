/* C++ Assembler Interpreter
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_FACTORY_HPP
#define ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_FACTORY_HPP

#include <cstdint>
#include <unordered_map>

#include "arch/common/abstract-instruction-node-factory.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/instruction-information.hpp"
#include "arch/common/instruction-set.hpp"

namespace riscv {

/**
 * \brief The InstructionNodeFactory class is a
 * concrete Implementation of AbstractInstructionNodeFactory for the RISC-V
 * architecture.
 * \see AbstractInstructionNodeFactory
 */
class InstructionNodeFactory : public AbstractInstructionNodeFactory {
 public:
  using InstructionMap =
      std::unordered_map<std::string,
                         std::function<std::unique_ptr<AbstractSyntaxTreeNode>(
                             InstructionInformation &)>>;
  /*! Word size constant to be expected when using 32bit instructions*/
  static constexpr Architecture::word_size_t RV32 = 32;
  /*! Internal integer type to represent 32bit for arithmetic operations*/
  using RV32_integral_t = uint32_t;
  /*Internal integer type to represent signed 32bit for arithmetic operations*/
  using RV32_signed_integral_t = int32_t;
  /*! Word size constant to be expected when using 64bit instructions*/
  static constexpr Architecture::word_size_t RV64 = 64;
  /*! Internal integer type to represent 64bit for arithmetic operations*/
  using RV64_integral_t = uint64_t;
  /*Internal integer type to represent signed 64bit for arithmetic operations*/
  using RV64_signed_integral_t = int64_t;

  /**
   * \brief InstructionNodeFactory
   * Creates a Instruction Node Factory for RISC-V architecture
   */
  InstructionNodeFactory(const InstructionSet &instructions,
                         const Architecture &architecture)
  : _instrSet(instructions) {
    assert(_instrSet.isValid());
    initializeInstructionMap(architecture);
  }

  /** Default constructed copy constructor */
  InstructionNodeFactory(InstructionNodeFactory &copy) = default;

  /** Default constructed move constructor */
  InstructionNodeFactory(InstructionNodeFactory &&move) = default;

  /**
   * \brief createInstructionNode
   * Creates and returns a RISC-V Instruction Node for a valid input token, or
   * nullptr if the token cannot be mapped to a implemented RISC-V instruction
   * \param token
   * \return std::uniqe_ptr pointing to the newly created instruction node, or
   * nullptr if the token cannot be mapped to a implemented RISC-V instruction
   */
  virtual std::unique_ptr<AbstractSyntaxTreeNode> createInstructionNode(
      const std::string &token) const override;

  ~InstructionNodeFactory() = default;

  /** Word size constant to be expected when using 32bit instructions*/
  static constexpr Architecture::word_size_t RV32 = 32;
  /** Internal integer type to represent 32bit for unsigned arithmetic
   * operations*/
  using RV32_integral_t = uint32_t;
  /** Internal integer type to represent 32bit for signed arithmetic
   * operations*/
  using RV32_signed_integral_t = int32_t;
  /** Word size constant to be expected when using 64bit instructions*/
  static constexpr Architecture::word_size_t RV64 = 64;
  /** Internal integer type to represent 64bit for unsigned arithmetic
   * operations*/
  using RV64_integral_t = uint64_t;
  /** Internal integer type to represent 64bit for signed arithmetic
   * operations*/
  using RV64_signed_integral_t = int64_t;

 private:
  /**
   * \brief initializeInstructionMap
   * Fills instructionMap with values.
   * Use lambda-functions with InstructionInformation as parameter and return
   * type std::unique_ptr<AbstractSyntaxTreeNode> as value.
   * Use lowercase instruction identifier as key.
   * \param architecture The architecture currently used. With this the factory
   * can determine, for what word size instructions are created
   */
  void initializeInstructionMap(const Architecture &architecture);

  /**
   * Checks if the given architecture has the 'M' Multiplication/Division
   * Extension loaded.
   * If so, all instructions of the extension are filled into the InstructionMap
   * \param architecture current loaded architecture
   * \param wordsize The wordsize of the architecture (e.g. 32 or 64bit)
   */
  void initializeMExtensionIfPresent(const Architecture &architecture,
                                     Architecture::word_size_t wordsize);

  /**
   * Table, that maps the instruction identifier (e.g. the token "add" for
   * Addition) to a function that creates the special instruction node (e.g.
   * AddInstructionNode)
   */
  InstructionMap _instructionMap;

  /**
   * Description of all instructions that can be created by this factory
   */
  InstructionSet _instrSet;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_INSTRUCTION_NODE_FACTORY_HPP */
