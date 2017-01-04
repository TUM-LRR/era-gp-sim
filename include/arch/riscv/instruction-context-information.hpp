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
#ifndef ERAGPSIM_ARCH_RISCV_INSTRUCTIONCONTEXTINFORMATION_HPP
#define ERAGPSIM_ARCH_RISCV_INSTRUCTIONCONTEXTINFORMATION_HPP

#include <QtGlobal>
#include <unordered_map>

#include "arch/common/architecture.hpp"
#include "common/translateable.hpp"

// Define a shorter, specialized version of QT_TRANSLATE_NOOP
#define RISCV_TR(textToTranslate) \
  QT_TRANSLATE_NOOP("RISCV instruction help text", textToTranslate)

/**
 * This class is a collection of translateable help texts/documentation for
 * RISCV instructions
 */
class InstructionContextInformation {
 public:
  /**
 * Constructs the documentation collection with the given architecture.
 * As the Translateable text parts are freshly made, requiring many string
 * operations,
 * this may be slow
 * \param architecture
 */
  InstructionContextInformation(const Architecture& architecture);

  /**
   * Retrieves and returns the documentation/context information for the given
   * mnemonic
   * \param mnemonic The mnemonic identifying the instruction
   * \return A reference to a Translateable containing the context information
   */
  const Translateable& getContextInformation(const std::string& mnemonic) const;

  /**
   * Retrieves and returns the documentation/context information for the given
   * instruction information
   * \param instructionInfo The instruction information object identifying the
   * instruction
   * \return A reference to a Translateable containing the context information
   */
  const Translateable& getContextInformation(
      const InstructionInformation& instructionInfo) const;

  /**
   * Checks if a documentation/context information is present for the given
   * mnemonic
   * \param mnemonic The mnemonic identifying the instruction
   * \return true, if an entry is available, otherwise false
   */
  bool isContextInformationAvailable(const std::string& mnemonic) const;

  /**
   * Checks if a documentation/context information is present for the given
   * instruction information
   * \param instructionInfo The instruction information object identifying the
   * instruction
   * \return true, if an entry is available, otherwise false
   */
  bool isContextInformationAvailable(
      const InstructionInformation& instructionInfo) const;

 private:
  using Table = std::unordered_map<std::string, Translateable>;

  /**
   * Fills (creates and puts the Translateables into) the map.
   * Only the information for instructions present in the given architecture is
   * generated
   * \param architecture The architecture that defines the instructions
   */
  void _fill(const Architecture& architecture);

  /**
   * Adds a key-value pair to the map
   * \param key The instruction name
   * \param value A Translateable text
   */
  void _add(const std::string& key, const Translateable& value);

  /**
   * A convenience function for a defining an arithmetic instruction
   * \param mnemonic The instruction mnemonic
   * \param operationSign The operator used in the pseudo-code description
   * \param description A description of the behaviour of the instruction
   * \param specialImmediateOperandDesc (optional) A description for the last
   * operand (the immediate operand). Defaults to "A 12bit signed immediate2
   */
  void _arithmeticInstruction(const std::string& mnemonic,
                              const std::string& operationSign,
                              const std::string& description,
                              const std::string& specialImmediateOperandDesc =
                                  RISCV_TR("A 12 bit signed immediate"),
                              const Optional<int> range = Optional<int>(),
                              const Optional<bool> isSigned = Optional<bool>(true));

  /**
   * A convenience function for a defining an arithmetic instruction.
   * This call defines a arithmetic instruction documentation for the register
   * and the immediate version of the instruction
   * \param mnemonic The instruction mnemonic
   * \param operationSign The operator used in the pseudo-code description
   * \param description A description of the behaviour of the instruction
   * \param specialImmediateOperandDesc (optional) A description for the last
   * operand (the immediate operand). Defaults to "A 12bit signed immediate2
   */
  void _arithmeticInstructionI(const std::string& mnemonic,
                               const std::string& operationSign,
                               const std::string& description,
                               const std::string& specialImmediateOperandDesc =
                                   RISCV_TR("A 12 bit signed immediate"),
                               int range=12, bool isSigned=true);

  /**
   * A convenience function for a defining a word arithmetic instruction
   * \param mnemonic The instruction mnemonic
   * \param operationSign The operator used in the pseudo-code description
   * \param description A description of the behaviour of the instruction
   * \param specialImmediateOperandDesc (optional) A description for the last
   * operand (the immediate operand). Defaults to "A 12bit signed immediate2
   */
  void _arithmeticInstructionW(const std::string& mnemonic,
                               const std::string& operationSign,
                               const std::string& description,
                               const std::string& specialImmediateOperandDesc =
                                   RISCV_TR("A 12 bit signed immediate"));

  /**
   * A convenience function for a defining a word arithmetic instruction.
   * This call defines a word instruction documentation for the register and the
   * immediate version of the word instruction
   * \param mnemonic The instruction mnemonic
   * \param operationSign The operator used in the pseudo-code description
   * \param description A description of the behaviour of the instruction
   * \param specialImmediateOperandDesc (optional) A description for the last
   * operand (the immediate operand). Defaults to "A 12bit signed immediate2
   */
  void _arithmeticInstructionIW(const std::string& mnemonic,
                                const std::string& operationSign,
                                const std::string& description,
                                const std::string& specialImmediateOperandDesc =
                                    RISCV_TR("A 12 bit signed immediate"),
                                int range=12, bool isSigned=true);

  /**
   * A convenience function for defining a load instruction documentation
   * \param mnemonic The instruction mnemonic
   * \param sizeDesc A string description of the load size (like byte, word,
   * double-word, etc.)
   * \param size The load size in bits
   */
  void _loadInstruction(const std::string& mnemonic,
                        const std::string& sizeDesc, int size);

  /**
   * A convenience function for defining a store instruction documentation
   * \param mnemonic The instruction mnemonic
   * \param sizeDesc A string description of the stored size (like byte, word,
   * double-word, etc.)
   * \param size The stored size in bits
   */
  void _storeInstruction(const std::string& mnemonic,
                         const std::string& sizeDesc, int size);

  /**
   * A convenience function for defining a branch instruction documentation
   * \param mnemonic The instruction mnemonic
   * \param condition A string description of the condition when the branch is
   * taken
   * \param operation The pseudo-code operator describing the comparison
   * \param compareType An optional description of the signedness of the
   * comparison
   */
  void _branchInstruction(const std::string& mnemonic,
                          const std::string& condition,
                          const std::string& operation,
                          const std::string& compareType = std::string());

  /**
   * A convenience function for defining a instruction from RISCV Extension M
   * \param mnemonic The instruction mnemonic
   * \param operationSign The pseudo-code operator for the arithmetic operation
   * \param description A description of the behaviour of the instruction
   * \param operand1Desc A description of the first operand
   * \param operand2Desc A description of the second operand
   * \param resultPart An optional description about the result of the operation
   */
  void _mInstruction(const std::string& mnemonic,
                     const std::string& operationSign,
                     const std::string& description,
                     const std::string& operand1Desc,
                     const std::string& operand2Desc,
                     const std::string& resultPart = std::string());

  bool _isImmediateInstruction(const std::string& mnemonic) const;

  Table _table;
  /** The architecture size in bits */
  std::string _architectureSize;
  bool _is64BitArchitecture;
};

#endif  // ERAGPSIM_ARCH_RISCV_INSTRUCTIONCONTEXTINFORMATION_HPP
