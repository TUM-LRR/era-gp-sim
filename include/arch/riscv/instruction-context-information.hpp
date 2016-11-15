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

#include <unordered_map>

#include "arch/common/architecture.hpp"
#include "common/translateable.hpp"

//Define a shorter, specialized version of QT_TRANSLATE_NOOP
#define RISCV_TR(textToTranslate) QT_TRANSLATE_NOOP("RISCV instruction help text", textToTranslate)

class InstructionContextInformation {
 public:
  InstructionContextInformation(const Architecture& architecture);

  const Translateable &getContextInformation(const std::string& mnemonic) const;

  const Translateable &getContextInformation(
      const InstructionInformation& instructionInfo) const;

  bool isContextInformationAvailable(const std::string& mnemonic) const;

  bool isContextInformationAvailable(const InstructionInformation& instructionInfo) const;

 private:
  using Table = std::unordered_map<std::string, Translateable>;

  void _gatherInfo(const Architecture& architecture);

  void _fill(const Architecture& architecture);

  void _add(const std::string& key, const Translateable& value);

  void _arithmeticInstruction(const std::string& mnemonic, const std::string& operationSign,
      const std::string& description,
      const std::string& specialImmediateOperandDesc = RISCV_TR("A 12 bit signed immediate"));

  void _arithmeticInstructionI(const std::string& mnemonic, const std::string& operationSign,
                               const std::string& description,
                               const std::string& specialImmediateOperandDesc = RISCV_TR("A 12 bit signed immediate"));

  void _arithmeticInstructionW(const std::string& mnemonic, const std::string& operationSign,
                               const std::string& description,
                               const std::string& specialImmediateOperandDesc = RISCV_TR("A 12 bit signed immediate"));

  void _arithmeticInstructionIW(const std::string& mnemonic, const std::string& operationSign,
                               const std::string& description,
                               const std::string& specialImmediateOperandDesc = RISCV_TR("A 12 bit signed immediate"));

  void _loadInstruction(const std::string& mnemonic, const std::string& sizeDesc, int size);

  void _storeInstruction(const std::string& mnemonic, const std::string& sizeDesc, int size);

  void _branchInstruction(const std::string& mnemonic, const std::string& condition, const std::string& operation, const std::string &compareType = std::string());

  void _mInstruction(const std::string& mnemonic, const std::string& operationSign, const std::string& description, const std::string& operand1Desc, const std::string& operand2Desc, const std::string &resultPart = std::string());

  Table _table;
  std::string _architectureSize;
  bool _is64BitArchitecture;
};

#endif  // ERAGPSIM_ARCH_RISCV_INSTRUCTIONCONTEXTINFORMATION_HPP
