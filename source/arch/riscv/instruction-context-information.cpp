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
#include <iostream>
#include "arch/riscv/documentation-builder.hpp"

InstructionContextInformation::InstructionContextInformation(
    const Architecture &architecture) {
  _architectureSize = "(" + std::to_string(architecture.getWordSize()) + "bit)";
  _is64BitArchitecture = architecture.getWordSize() == 64;
  _fill(architecture);
}

const Translateable &InstructionContextInformation::getContextInformation(
    const InstructionInformation &instructionInfo) const {
  return getContextInformation(instructionInfo.getMnemonic());
}

const Translateable &InstructionContextInformation::getContextInformation(
    const std::string &mnemonic) const {
  std::string key = Utility::toLower(mnemonic);
  assert::that(isContextInformationAvailable(key));
  return _table.at(key);
}

bool InstructionContextInformation::isContextInformationAvailable(
    const std::string &mnemonic) const {
  std::string key = Utility::toLower(mnemonic);
  return _table.find(key) != _table.end();
}

bool InstructionContextInformation::isContextInformationAvailable(
    const InstructionInformation &instructionInfo) const {
  return isContextInformationAvailable(instructionInfo.getMnemonic());
}

void InstructionContextInformation::_fill(const Architecture &architecture) {
  // Add arithmetic instructions
  _arithmeticInstructionI(
      "add", "+", RISCV_TR("Calculates the sum of %1 and %2 and stores the "
                           "result in the destination register"));
  _arithmeticInstruction(
      "sub", "-", RISCV_TR("Subtracts %2 from %1 and stores the result in the "
                           "destination register"));
  _arithmeticInstructionI(
      "and", RISCV_TR("AND"),
      RISCV_TR("Calculates a bitwise AND of %1 and %2 and stores "
               "the result in the destination register"));
  _arithmeticInstructionI(
      "or", RISCV_TR("OR"),
      RISCV_TR("Calculates a bitwise OR of %1 and %2 and stores "
               "the result in the destination register"));
  _arithmeticInstructionI(
      "xor", RISCV_TR("XOR"),
      RISCV_TR("Calculates a bitwise XOR of %1 and %2 and stores "
               "the result in the destination register"));
  _arithmeticInstructionI(
      "sll", "<<",
      RISCV_TR("(Logical) shifts the content of %1 by %2 bits to "
               "the left, inserting 0s in bit 0.<br>Note that only "
               "the lower 5bit of %2 determine the shift amount"),
      RISCV_TR("A 5bit unsigned immediate"));
  _arithmeticInstructionI(
      "srl", ">>",
      RISCV_TR("(Logical) shifts the content of %1 by %2 bits to "
               "the right, inserting 0s in the last bit.<br>Note "
               "that only the lower 5bit of %2 determine the shift "
               "amount"),
      RISCV_TR("A 5bit unsigned immediate"));
  _arithmeticInstructionI(
      "sra", "<<",
      RISCV_TR("(Arithmetical) shifts the content of %1 by %2 bits "
               "to the right, inserting the sign bit.<br>Note that "
               "only the lower 5bit of %2 determine the shift "
               "amount"),
      RISCV_TR("A 5bit unsigned immediate"));

  _add("slt",
       DocumentationBuilder()
           .instruction("slt")
           .shortDescription("rd = (rs1 < rs2) ? 1 : 0")
           .shortSyntax({"rd", "rs1", "rs2"})
           .operandDescription("rd", RISCV_TR("The destination register"))
           .operandDescription("rs1", RISCV_TR("First source register"))
           .operandDescription("rs2", RISCV_TR("Second source register"))
           .detailDescription(
               RISCV_TR("Performs a signed comparison of rs1 and rs2. If "
                        "rs1 contains a number "
                        "that is less than rs2, the destination register "
                        "is filled with the value 1, if not, the "
                        "destination register is fills with the value 0"))
           .build());
  _add("sltu",
       DocumentationBuilder()
           .instruction("sltu")
           .shortDescription("rd = (rs1 < rs2) ? 1 : 0")
           .shortSyntax({"rd", "rs1", "rs2"})
           .operandDescription("rd", RISCV_TR("The destination register"))
           .operandDescription("rs1", RISCV_TR("First source register"))
           .operandDescription("rs2", RISCV_TR("Second source register"))
           .detailDescription(
               RISCV_TR("Performs an unsigned comparison of rs1 and rs2. "
                        "If rs1 contains a number "
                        "that is less than rs2, the destination register "
                        "is filled with the value 1, if not, the "
                        "destination register is fills with the value 0"))
           .build());
  _add("slti",
       DocumentationBuilder()
           .instruction("slti")
           .shortDescription("rd = (rs < imm) ? 1 : 0")
           .shortSyntax({"rd", "rs", "imm"})
           .operandDescription("rd", RISCV_TR("The destination register"))
           .operandDescription("rs", RISCV_TR("The source register"))
           .operandDescription("imm", RISCV_TR("A signed 12 bit immediate"))
           .detailDescription(
               RISCV_TR("Performs a signed comparison of rs and imm. If "
                        "rs contains a number "
                        "that is less than imm, the destination register "
                        "is filled with the value 1, if not, the "
                        "destination register is fills with the value 0"))
           .build());
  _add("sltiu",
       DocumentationBuilder()
           .instruction("sltiu")
           .shortDescription("rd = (rs < imm) ? 1 : 0")
           .shortSyntax({"rd", "rs", "imm"})
           .operandDescription("rd", RISCV_TR("The destination register"))
           .operandDescription("rs", RISCV_TR("The source register"))
           .operandDescription("imm", RISCV_TR("A signed 12 bit immediate"))
           .detailDescription(
               RISCV_TR("Performs an unsigned comparison of rs and imm. "
                        "If rs contains a number "
                        "that is less than imm, the destination register "
                        "is filled with the value 1, if not, the "
                        "destination register is filled with the value 0"))
           .build());

  // Load instructions
  _loadInstruction("lb", RISCV_TR("byte"), 8);
  _loadInstruction("lbu", RISCV_TR("unsigned byte"), 8);
  _loadInstruction("lh", RISCV_TR("half word"), 16);
  _loadInstruction("lhu", RISCV_TR("unsigned half word"), 16);
  _loadInstruction("lw", RISCV_TR("word"), 32);
  if (_is64BitArchitecture) {
    _loadInstruction("lwu", RISCV_TR("unsigned word"), 32);
    _loadInstruction("ld", RISCV_TR("double word"), 64);
  }

  // Store instructions
  _storeInstruction("sb", RISCV_TR("byte"), 8);
  _storeInstruction("sh", RISCV_TR("half word"), 16);
  _storeInstruction("sw", RISCV_TR("word"), 32);
  if (_is64BitArchitecture) {
    _storeInstruction("sd", RISCV_TR("double word"), 64);
  }

  // Lui & AUIPC
  _add("lui",
       DocumentationBuilder()
           .instruction("lui")
           .shortSyntax({"rd", "imm"})
           .operandDescription("rd", RISCV_TR("The destination register"))
           .operandDescription("imm", RISCV_TR("A signed 20bit immediate"))
           .shortDescription("rd = imm << 12")
           .detailDescription(
               RISCV_TR("Places the sign extended immediate into bit 31 "
                        "to 12. Bits 11 to 0 are filled with 0s"))
           .build());
  _add("auipc",
       DocumentationBuilder()
           .instruction("auipc")
           .shortSyntax({"rd", "imm"})
           .operandDescription("rd", RISCV_TR("The destination register"))
           .operandDescription("imm", RISCV_TR("A signed 20bit immediate"))
           .shortDescription("rd = (imm << 12) + pc")
           .detailDescription(
               RISCV_TR("Places the sign extended immediate into bit 31 "
                        "to 12. Bits 11 to 0 are filled with 0s. The "
                        "the current program counter is added"))
           .build());

  // jump instructions
  _add("jal",
       DocumentationBuilder()
           .instruction("jal")
           .shortSyntax({"rd", "offset"})
           .operandDescription("rd", RISCV_TR("Register where the address of "
                                              "the next instruction is stored"))
           .operandDescription("offset",
                               RISCV_TR("A signed 20bit offset that is used to "
                                        "calculate the jump address in bytes"))
           .shortDescription("rd = pc+4<br>pc+=2*offset")
           .detailDescription(
               RISCV_TR("Performs a jump to a label or an instruction "
                        "address determined by program counter + "
                        "2xoffset. The instruction address following the "
                        "jump instruction is saved in the rd register"))
           .build());
  _add("jalr",
       DocumentationBuilder()
           .instruction("jalr")
           .shortSyntax({"rd", "base", "offset"})
           .operandDescription("rd", RISCV_TR("Register where the address of "
                                              "the next instruction is stored"))
           .operandDescription("base",
                               RISCV_TR("A base register used for calculating "
                                        "the jump address in bytes"))
           .operandDescription(
               "offset", RISCV_TR("A signed 12 bit offset that is used to "
                                  "calculate the jump address in bytes"))
           .shortDescription("rd = pc+4<br>pc=base+offset")
           .detailDescription(
               RISCV_TR("Performs a jump to a label or an instruction "
                        "address determined by adding base and offset. "
                        "The instruction address following the jump "
                        "instruction is saved in the rd register"))
           .build());
  // branch instructions
  _branchInstruction("beq", RISCV_TR("equal to"), "==");
  _branchInstruction("bne", RISCV_TR("not equal to"), "!=");
  _branchInstruction("blt", RISCV_TR("less than"), "<",
                     RISCV_TR(" (signed compare)"));
  _branchInstruction("bltu", RISCV_TR("less than"),
                     RISCV_TR(" (unsigned compare)"));
  _branchInstruction("bge", RISCV_TR("greater or equal to"), ">=",
                     RISCV_TR(" (signed compare)"));
  _branchInstruction("bgeu", RISCV_TR("greater or equal to"), ">=",
                     RISCV_TR(" (unsigned compare)"));

  // sleep & crash
  _add("simusleep", DocumentationBuilder()
                        .instruction("simusleep")
                        .shortSyntax({"ms"})
                        .operandDescription(
                            "ms", RISCV_TR("The time to sleep in milliseconds"))
                        .detailDescription(RISCV_TR(
                            "Holds the execution of the next instruction at "
                            "least ms milliseconds"))
                        .build());
  _add(
      "simucrash",
      DocumentationBuilder()
          .instruction("simucrash")
          .shortSyntax({"msg"})
          .operandDescription(
              "msg",
              RISCV_TR("Message to be shown when this instruction is executed"))
          .detailDescription(
              RISCV_TR("Terminates the running program showing msg"))
          .build());

  // RV64I
  if (_is64BitArchitecture) {
    _arithmeticInstructionIW(
        "add", "+",
        RISCV_TR("Calculates the sum of the lower 32bit of %1 and the lower "
                 "32bit of %2 "
                 "and stores the "
                 "sign extended 64bit result in the destination register"));
    _arithmeticInstructionW(
        "sub", "-", RISCV_TR("Subtracts the lower 32bit of %2 from the lower "
                             "32bit of %1 and stores the sign extended 64bit "
                             "result in the "
                             "destination register"));
    _arithmeticInstructionIW(
        "and", RISCV_TR("AND"),
        RISCV_TR(
            "Calculates a bitwise AND of the lower 32bit of %1 and the lower "
            "32bit of %2 and stores "
            "the sign extended 64 result in the destination register"));
    _arithmeticInstructionIW(
        "or", RISCV_TR("OR"),
        RISCV_TR("Calculates a bitwise OR of the lower 32bit of %1 and the "
                 "lower 32bit "
                 "of %2 and stores "
                 "the sign extended 64bit result in the destination register"));
    _arithmeticInstructionIW(
        "xor", RISCV_TR("XOR"),
        RISCV_TR(
            "Calculates a bitwise XOR of the lower 32bit of %1 and the lower "
            "32bit of %2 and stores "
            "the sign extended 64bit result in the destination register"));
    _arithmeticInstructionIW(
        "sll", "<<",
        RISCV_TR("(Logical) shifts the content of the lower 32bit of %1 by %2 "
                 "bits to "
                 "the left, inserting 0s in bit 0.<br>Note that only "
                 "the lower 5bit of %2 determine the shift amount. The result "
                 "is sign "
                 "extended to 64bit"),
        RISCV_TR("A 5bit unsigned immediate"));
    _arithmeticInstructionIW(
        "srl", ">>",
        RISCV_TR("(Logical) shifts the content of the lower 32bit of %1 by %2 "
                 "bits to "
                 "the right, inserting 0s in the last bit.<br>Note "
                 "that only the lower 5bit of %2 determine the shift "
                 "amount. The result is sign extended to 64bit"),
        RISCV_TR("A 5bit unsigned immediate"));
    _arithmeticInstructionIW(
        "sra", "<<",
        RISCV_TR("(Arithmetical) shifts the content of the lower 32bit of %1 "
                 "by %2 bits "
                 "to the right, inserting the sign bit.<br>Note that "
                 "only the lower 5bit of %2 determine the shift "
                 "amount. The result is sign extended to 64bit"),
        RISCV_TR("A 5bit unsigned immediate"));
  }

  // M-Extension
  if (architecture.getInstructions().hasInstruction("mul")) {
    _mInstruction("mul", "*", RISCV_TR("a signed x signed multiplication"),
                  RISCV_TR("register used for the multiplicand"),
                  RISCV_TR("register used for the multiplier"),
                  RISCV_TR("lower"));
    _mInstruction("mulh", "*", RISCV_TR("a signed x signed multiplication"),
                  RISCV_TR("register used for the multiplicand"),
                  RISCV_TR("register used for the multiplier"),
                  RISCV_TR("upper"));
    _mInstruction("mulhu", "*", RISCV_TR("an unsigned x unsigned multiplication"),
                  RISCV_TR("register used for the multiplicand"),
                  RISCV_TR("register used for the multiplier"),
                  RISCV_TR("upper"));
    _mInstruction("mulhsu", "*", RISCV_TR("a signed x unsigned multiplication"),
                  RISCV_TR("register used for the multiplicand"),
                  RISCV_TR("register used for the multiplier"),
                  RISCV_TR("upper"));

    _mInstruction("div", "/", RISCV_TR("a signed-signed division"),
                  RISCV_TR("register used as dividend"),
                  RISCV_TR("register used as divisor"));
    _mInstruction("divu", "/", RISCV_TR("an unsigned-unsigned division"),
                  RISCV_TR("register used as dividend"),
                  RISCV_TR("register used as divisor"));
    _mInstruction("rem", "%", RISCV_TR("a signed remainder operation"),
                  RISCV_TR("register used as dividend"),
                  RISCV_TR("register used as divisor"));
    _mInstruction("remu", "%", RISCV_TR("an unsigned remainder operation"),
                  RISCV_TR("register used as dividend"),
                  RISCV_TR("register used as divisor"));

    if (_is64BitArchitecture) {
      _mInstruction(
          "mulw", "*", RISCV_TR("a signed 32bit x signed 32bit multiplication"),
          RISCV_TR("register used for the multiplicand"),
          RISCV_TR("register used for the multiplier"), RISCV_TR("lower"));
      _mInstruction("divw", "/",
                    RISCV_TR("a signed 32bit -signed 32bit division"),
                    RISCV_TR("register used as dividend"),
                    RISCV_TR("register used as divisor"));
      _mInstruction("divuw", "/",
                    RISCV_TR("an unsigned 32bit -unsigned 32bit division"),
                    RISCV_TR("register used as dividend"),
                    RISCV_TR("register used as divisor"));
      _mInstruction("remw", "%", RISCV_TR("a signed 32bit remainder operation"),
                    RISCV_TR("register used as dividend"),
                    RISCV_TR("register used as divisor"));
      _mInstruction("remuw", "%",
                    RISCV_TR("an unsigned 32bit remainder operation"),
                    RISCV_TR("register used as dividend"),
                    RISCV_TR("register used as divisor"));
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

void InstructionContextInformation::_arithmeticInstructionW(
    const std::string &mnemonic, const std::string &operationSign,
    const std::string &description,
    const std::string &specialImmediateOperandDesc) {
  _arithmeticInstruction(mnemonic + 'w', operationSign, description,
                         specialImmediateOperandDesc);
}

void InstructionContextInformation::_arithmeticInstructionIW(
    const std::string &mnemonic, const std::string &operationSign,
    const std::string &description,
    const std::string &specialImmediateOperandDesc) {
  _arithmeticInstruction(mnemonic + 'w', operationSign, description,
                         specialImmediateOperandDesc);
  _arithmeticInstruction(mnemonic + "iw", operationSign, description,
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
  builder.operandDescription(rd, RISCV_TR("The destination register"))
      .operandDescription(op1, isImmediate
                                   ? RISCV_TR("The operand register")
                                   : RISCV_TR("The first operand register"))
      .operandDescription(op2, isImmediate
                                   ? specialImmediateOperandDesc
                                   : RISCV_TR("The second operand register"));
  auto detailDesc = std::make_shared<Translateable>(description);
  detailDesc->addOperand(op1);
  detailDesc->addOperand(op2);
  builder.detailDescription(detailDesc);
  _add(mnemonic, builder.build());
}

void InstructionContextInformation::_loadInstruction(
    const std::string &mnemonic, const std::string &sizeDesc, int size) {
  DocumentationBuilder builder;
  builder.instruction(mnemonic);
  builder.shortSyntax({"rd", "base", "offset"})
      .shortDescription("rd = [base+offset]");
  builder.operandDescription(
      "rd", RISCV_TR("The register, the loaded value is stored to"));
  builder.operandDescription(
      "base",
      RISCV_TR("A base register used for calculating the memory address"));
  builder.operandDescription(
      "offset", RISCV_TR("A 12 bit signed immediate offset that is used to "
                         "calculate the memory address"));
  auto detailTranslate = std::make_shared<Translateable>(
      RISCV_TR("Loads one %1(%2 bit) into the register rd%3%4"));
  detailTranslate->addOperand(sizeDesc);
  detailTranslate->addOperand(std::to_string(size));
  if (mnemonic.back() != 'u' && mnemonic.back() != 'U') {
    detailTranslate->addOperand(RISCV_TR(
        " performing a sign extension on the whole register if needed"));
  } else {
    detailTranslate->addOperand("");
  }
  detailTranslate->addOperand(RISCV_TR(
      ". The memory address is determined by adding base and offset."));
  builder.detailDescription(detailTranslate);
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
          "base",
          RISCV_TR("A base register used for calculating the memory address"))
      .operandDescription("source",
                          RISCV_TR("The register whose value will be stored"))
      .operandDescription(
          "offset", RISCV_TR("A 12 bit signed immediate offset that us used to "
                             "calculate the memory address"));
  auto detail = std::make_shared<Translateable>(
      RISCV_TR("Stores a %1 from the source register (using bit 0 to %2). The "
               "memory address is determined by adding base and offset"));
  detail->addOperand(sizeDesc);
  detail->addOperand(std::to_string(size - 1));
  builder.detailDescription(detail);
  _add(mnemonic, builder.build());
}

void InstructionContextInformation::_branchInstruction(
    const std::string &mnemonic, const std::string &condition,
    const std::string &operation, const std::string &compareType) {
  DocumentationBuilder builder;
  builder.instruction(mnemonic);
  builder.shortSyntax({"rs1", "rs2", "offset"})
      .shortDescription("pc = (rs1 " + operation + " rs2) ? pc+offset : pc+4");
  builder.operandDescription("rs1", RISCV_TR("First register to compare"))
      .operandDescription("rs2", RISCV_TR("Second register to compare"))
      .operandDescription(
          "offset", RISCV_TR("A 12 bit signed immediate offset that is used to "
                             "calculate the branch address"));
  auto detail = std::make_shared<Translateable>(
      RISCV_TR("Compares rs1 and rs2. If rs1 is %1 rs2%2, the program jumps to "
               "the label or instruction address determined by adding the "
               "program counter to 2x offset. If not no jump is made."));
  detail->addOperand(condition);
  detail->addOperand(compareType);
  builder.detailDescription(detail);
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
      .operandDescription(
          "rd", RISCV_TR("Destination register where the result is stored"))
      .operandDescription("rs1", operand1Desc)
      .operandDescription("rs2", operand2Desc);
  auto detail = std::make_shared<Translateable>(
      RISCV_TR("Performs %1 with rs1 and rs2 and stores the %2%3 of the "
               "result in the destination register rd"));
  detail->addOperand(description);
  detail->addOperand(resultPart);
  detail->addOperand(_architectureSize);
  builder.detailDescription(detail);
  _add(mnemonic, builder.build());
}

void InstructionContextInformation::_add(const std::string &key,
                                         const Translateable &value) {
  _table.emplace(key, value);
}
