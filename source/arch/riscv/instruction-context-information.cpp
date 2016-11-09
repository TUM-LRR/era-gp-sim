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

#include "arch/riscv/instruction-context-information.hpp"
#include "arch/riscv/documentation-builder.hpp"

InstructionContextInformation::InstructionContextInformation(
    const Architecture &architecture) {
  _gatherInfo(architecture);
  _fill(architecture);
}

const std::string &InstructionContextInformation::getContextInformation(
    const InstructionInformation &instructionInfo) const {
  return getContextInformation(instructionInfo.getMnemonic());
}

const std::string &InstructionContextInformation::getContextInformation(
    const std::string &mnemonic) const {
  std::string key = Utility::toLower(mnemonic);
  assert::that(_table.find(key) != _table.end());
  return _table.at(key);
}

void InstructionContextInformation::_gatherInfo(
    const Architecture &architecture) {
  _architectureSize = "(" + std::to_string(architecture.getWordSize()) + "bit)";
  _is64BitArchitecture = architecture.getWordSize() == 64;
}

void InstructionContextInformation::_fill(const Architecture &architecture) {
  // Add arithmetic instructions
  _arithmeticInstructionI("add", "+",
                          "Calculates the sum of &1 and &2 and stores the "
                          "result in the destination register");
  _arithmeticInstruction("sub", "-",
                         "Subtracts &2 from &s and stores the result in the "
                         "destionation register");
  _arithmeticInstructionI("and", "AND",
                          "Caclulates a bytewise AND of &1 and &2 and stores "
                          "the result in the destination register");
  _arithmeticInstructionI("or", "OR",
                          "Calculates a bytewise OR of &1 and &2 and stores "
                          "the result in the destination register");
  _arithmeticInstructionI("xor", "XOR",
                          "Calculates a bytewise XOR of &1 and &2 and stores "
                          "the result in the destination register");
  _arithmeticInstructionI("sll", "<<",
                          "(Logical) shifts the content of &1 by &2 bits to "
                          "the left, inserting 0s in bit 0.<br>Note that only "
                          "the lower 5bit of &2 determine the shift amount",
                          "A 5bit unsigned immediate");
  _arithmeticInstructionI("srl", ">>",
                          "(Logical) shifts the content of &1 by &2 bits to "
                          "the right, inserting 0s in the last bit.<br>Note "
                          "that only the lower 5bit of &2 determine the shift "
                          "amount",
                          "A 5bit unsigned immediate");
  _arithmeticInstructionI("sra", "<<",
                          "(Arithmetical) shifts the content of &1 by &2 bits "
                          "to the right, inserting the sign bit.<br>Note that "
                          "only the lower 5bit of &2 determine the shift "
                          "amount",
                          "A 5bit unsigned immediate");

  _add("slt",
       DocumentationBuilder()
           .instruction("slt")
           .shortDescription("rd = (rs1 < rs2) ? 1 : 0")
           .shortSyntax({"rd", "rs1", "rs2"})
           .operandDescription("rd", "The destination register")
           .operandDescription("rs1", "First source register")
           .operandDescription("rs2", "Second source register")
           .detailDescription("Performs a signed comparison of rs1 and rs2. If "
                              "rs1 contains a number "
                              "that is less than rs2, the destination register "
                              "is filled with the value 1, if not, the "
                              "destination register is filles with the value 0")
           .build());
  _add("sltu",
       DocumentationBuilder()
           .instruction("sltu")
           .shortDescription("rd = (rs1 < rs2) ? 1 : 0")
           .shortSyntax({"rd", "rs1", "rs2"})
           .operandDescription("rd", "The destination register")
           .operandDescription("rs1", "First source register")
           .operandDescription("rs2", "Second source register")
           .detailDescription("Performs an unsigned comparison of rs1 and rs2. "
                              "If rs1 contains a number "
                              "that is less than rs2, the destination register "
                              "is filled with the value 1, if not, the "
                              "destination register is filles with the value 0")
           .build());
  _add("slti",
       DocumentationBuilder()
           .instruction("slti")
           .shortDescription("rd = (rs < imm) ? 1 : 0")
           .shortSyntax({"rd", "rs", "imm"})
           .operandDescription("rd", "The destination register")
           .operandDescription("rs", "The source register")
           .operandDescription("imm", "A signed 12 bit immediate")
           .detailDescription("Performs a signed comparison of rs and imm. If "
                              "rs contains a number "
                              "that is less than imm, the destination register "
                              "is filled with the value 1, if not, the "
                              "destination register is filles with the value 0")
           .build());
  _add("sltiu",
       DocumentationBuilder()
           .instruction("sltiu")
           .shortDescription("rd = (rs < imm) ? 1 : 0")
           .shortSyntax({"rd", "rs", "imm"})
           .operandDescription("rd", "The destination register")
           .operandDescription("rs", "The source register")
           .operandDescription("imm", "A signed 12 bit immediate")
           .detailDescription("Performs an unsigned comparison of rs and imm. "
                              "If rs contains a number "
                              "that is less than imm, the destination register "
                              "is filled with the value 1, if not, the "
                              "destination register is filled with the value 0")
           .build());

  // Load instructions
  _loadInstruction("lb", "byte", 8);
  _loadInstruction("lbu", "unsigned byte", 8);
  _loadInstruction("lh", "half word", 16);
  _loadInstruction("lhu", "unsigned half word", 16);
  _loadInstruction("lw", "word", 32);
  if (_is64BitArchitecture) {
    _loadInstruction("lwu", "unsigned word", 32);
    _loadInstruction("ld", "double word", 64);
  }

  // Store instructions
  _storeInstruction("sb", "byte", 8);
  _storeInstruction("sh", "half word", 16);
  _storeInstruction("sw", "word", 32);
  if (_is64BitArchitecture) {
    _storeInstruction("sd", "double word", 64);
  }

  // Lui & AUIPC
  _add("lui",
       DocumentationBuilder()
           .instruction("lui")
           .shortSyntax({"rd", "imm"})
           .operandDescription("rd", "The destination register")
           .operandDescription("imm", "A signed 20bit immediate")
           .shortDescription("rd = imm << 12")
           .detailDescription("Places the sign extended immediate into bit 31 "
                              "to 12. Bits 11 to 0 are filled with 0s")
           .build());
  _add("auipc",
       DocumentationBuilder()
           .instruction("auipc")
           .shortSyntax({"rd", "imm"})
           .operandDescription("rd", "The destination register")
           .operandDescription("imm", "A signed 20bit immediate")
           .shortDescription("rd = (imm << 12) + pc")
           .detailDescription("Places the sign extended immediate into bit 31 "
                              "to 12. Bits 11 to 0 are filled with 0s. The "
                              "the current program counter is added")
           .build());

  // jump instructions
  _add("jal",
       DocumentationBuilder()
           .instruction("jal")
           .shortSyntax({"rd", "offset"})
           .operandDescription(
               "rd",
               "Register where the address of the next instruction is stored")
           .operandDescription("offset",
                               "A signed 20bit offset that is used to "
                               "calculate the jump address in bytes")
           .shortDescription("rd = pc+4<br>pc+=2*offset")
           .detailDescription("Performs a jump to a label or an instruction "
                              "address determined by program counter + "
                              "2xoffset. The instruction address following the "
                              "jump instruction is saved in the rd register")
           .build());
  _add("jalr",
       DocumentationBuilder()
           .instruction("jalr")
           .shortSyntax({"rd", "base", "offset"})
           .operandDescription(
               "rd",
               "Register where the address of the next instruction is stored")
           .operandDescription(
               "base",
               "A base register used for calculating the jump address in bytes")
           .operandDescription("offset",
                               "A signed 12 bit offset that is used to "
                               "calculate the jump adress in bytes")
           .shortDescription("rd = pc+4<br>pc=base+offset")
           .detailDescription("Performs a jump to a label or an instruction "
                              "address determined by adding base and offset. "
                              "The instruction address following the jump "
                              "instruction is saved in the rd register")
           .build());
  // branch instructions
  _branchInstruction("beq", "equal to", "==");
  _branchInstruction("bne", "not equal to", "!=");
  _branchInstruction("blt", "less than", "<", " (signed compare)");
  _branchInstruction("bltu", "less than", " (unsigned compare)");
  _branchInstruction("bge", "greater or equal to", ">=", " (signed compare)");
  _branchInstruction("bgeu", "greater or equal to", ">=", " (unsigned compare)");

  // sleep & crash
  _add("simusleep",
       DocumentationBuilder()
           .instruction("simusleep")
           .shortSyntax({"ms"})
           .operandDescription("ms", "The time to sleep in milliseconds")
           .detailDescription("Holds the execution of the next instruction at "
                              "least ms milliseconds")
           .build());
  _add("simucrash",
       DocumentationBuilder()
           .instruction("simucrash")
           .shortSyntax({"msg"})
           .operandDescription(
               "msg", "Message to be shown when this instruction is executed")
           .detailDescription("Terminates the running program showing msg")
           .build());

  // M-Extension
  if (architecture.getInstructions().hasInstruction("mul")) {
    _mInstruction("mul", "*", "signed x signed multiplication",
                  "register used for the multiplicand",
                  "register used for the multiplier", "lower");
    _mInstruction("mulh", "*", "signed x signed multiplication",
                  "register used for the multiplicand",
                  "register used for the multiplier", "upper");
    _mInstruction("mulhu", "*", "unsigned x unsigned multiplication",
                  "register used for the multiplicand",
                  "register used for the multiplier", "upper");
    _mInstruction("mulhsu", "*", "signed x unsigned multiplication",
                  "register used for the multiplicand",
                  "register used for the multiplier", "upper");

    _mInstruction("div", "/", "signed-signed division",
                  "register used as dividend", "register used as divisor");
    _mInstruction("divu", "/", "unsigned-unsigned division",
                  "register used as dividend", "register used as divisor");
    _mInstruction("rem", "%", "signed remainder operation",
                  "register used as dividend", "register used as divisor");
    _mInstruction("remu", "%", "unsigned remainder operation",
                  "register used as dividend", "register used as divisor");

    if (_is64BitArchitecture) {
      _mInstruction("mulw", "*", "signed 32bit x signed 32bit multiplication",
                    "register used for the multiplicand",
                    "register used for the multiplier", "lower");
      _mInstruction("divw", "/", "signed 32bit -signed 32bit division",
                    "register used as dividend", "register used as divisor");
      _mInstruction("divuw", "/", "unsigned 32bit -unsigned 32bit division",
                    "register used as dividend", "register used as divisor");
      _mInstruction("remw", "%", "signed 32bit remainder operation",
                    "register used as dividend", "register used as divisor");
      _mInstruction("remuw", "%", "unsigned 32bit remainder operation",
                    "register used as dividend", "register used as divisor");
    }
  }
}

void InstructionContextInformation::_arithmeticInstructionI(
    const std::string &mnemonic, const std::string &operationSign,
    const std::string &description,
    const std::string &specialImmediateOperandDesc) {
  _arithmeticInstruction(mnemonic, operationSign, description,
                         specialImmediateOperandDesc);
  _arithmeticInstruction(mnemonic + 'i', operationSign, description,
                         specialImmediateOperandDesc);
}

void InstructionContextInformation::_arithmeticInstruction(
    const std::string &mnemonic, const std::string &operationSign,
    const std::string &description,
    const std::string &specialImmediateOperandDesc) {
  DocumentationBuilder builder;
  builder.instruction(mnemonic);
  bool isImmediate = mnemonic.back() == 'i' || mnemonic.back() == 'I';
  std::string rd = "rd";
  std::string op1 = isImmediate ? "rs" : "rs1";
  std::string op2 = isImmediate ? "imm" : "rs2";
  builder.shortSyntax({rd, op1, op2})
      .shortDescription(rd + " = " + op1 + " " + operationSign + " " + op2);
  builder
      .operandDescription(rd, "The destination register " + _architectureSize)
      .operandDescription(
          op1, isImmediate ? "The operand register " + _architectureSize
                           : "The first operand register " + _architectureSize)
      .operandDescription(op2, isImmediate ? specialImmediateOperandDesc
                                           : "The second operand register " +
                                                 _architectureSize);

  builder.detailDescription(description);
  _add(mnemonic, builder.build());
}

void InstructionContextInformation::_loadInstruction(
    const std::string &mnemonic, const std::string &sizeDesc, int size) {
  DocumentationBuilder builder;
  builder.instruction(mnemonic);
  builder.shortSyntax({"rd", "base", "offset"})
      .shortDescription("rd = [base+offset]");
  builder.operandDescription("rd",
                             "The register, the loaded value is stored to");
  builder.operandDescription(
      "base", "A base register used for calculating the memory address");
  builder.operandDescription("offset",
                             "A 12 bit signed immediate offset that is used to "
                             "calculate the memory address");
  std::string detail = "Loads one " + sizeDesc + "(" + std::to_string(size) +
                       "bit) into the register rd";
  if (mnemonic.back() != 'u' && mnemonic.back() != 'U') {
    detail += " performing a sign extension on the whole register if needed";
  }
  detail +=
      ". The memory address is "
      "determined by adding base and offset.";
  builder.detailDescription(detail);
  _add(mnemonic, builder.build());
}

void InstructionContextInformation::_storeInstruction(
    const std::string &mnemonic, const std::string &sizeDesc, int size) {
  DocumentationBuilder builder;
  builder.instruction(mnemonic);
  builder.shortSyntax({"base", "source", "offset"})
      .shortDescription("[base+offset] = source");
  builder
      .operandDescription(
          "base", "A base register used for calculating the memory adress")
      .operandDescription("source", "The register whose value will be stored")
      .operandDescription("offset",
                          "A 12 bit signed immediate offset that us used to "
                          "calculate the memory adress");
  builder.detailDescription(
      "Stores a " + sizeDesc + " from the source register (using bit 0 to " +
      std::to_string(size - 1) +
      "). The memory adress is determined by adding base and offset");
  _add(mnemonic, builder.build());
}

void InstructionContextInformation::_branchInstruction(
    const std::string &mnemonic, const std::string &condition,
    const std::string &operation, const std::string &compareType) {
  DocumentationBuilder builder;
  builder.instruction(mnemonic);
  builder.shortSyntax({"rs1", "rs2", "offset"})
      .shortDescription("pc = (rs1 " + operation + " rs2) ? pc+offset : pc+4");
  builder.operandDescription("rs1", "First register to compare")
      .operandDescription("rs2", "Second register to compare")
      .operandDescription("offset",
                          "A 12 bit signed immediate offset that is used to "
                          "calculate the branch address");
  builder.detailDescription(
      "Compares rs1 and rs2. If rs1 is " + condition + " rs2" + compareType +
      ", the program jumps to the label or the instruction address determined "
      "by adding the program counter to 2x offset. If not no jump is made.");
  _add(mnemonic, builder.build());
}

void InstructionContextInformation::_mInstruction(
    const std::string &mnemonic, const std::string &operationSign,
    const std::string &description, const std::string &operand1Desc,
    const std::string &operand2Desc, const std::string &resultPart) {
  DocumentationBuilder builder;
  builder.instruction(mnemonic);
  builder.shortSyntax({"rd", "rs1", "rs2"})
      .shortDescription("rd = rs1 " + operationSign + " rs2");
  builder
      .operandDescription("rd",
                          "Destination register where the result is stored")
      .operandDescription("rs1", operand1Desc)
      .operandDescription("rs2", operand2Desc);
  builder.detailDescription("Performs a " + description +
                            " with rs1 and rs2 and stores the " + resultPart +
                            " " + _architectureSize +
                            " of the result in the destination register rd");
  _add(mnemonic, builder.build());
}

void InstructionContextInformation::_add(const std::string &key,
                                         const std::string &value) {
  _table[Utility::toLower(key)] = value;
}
