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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "parser/riscv-parser.hpp"

#include <iostream>
#include <regex>
#include <sstream>
#include "arch/common/architecture.hpp"
#include "arch/common/node-factory-collection-maker.hpp"
#include "arch/common/unit-information.hpp"
#include "parser/intermediate-instruction.hpp"
#include "parser/intermediate-representator.hpp"
#include "parser/riscv-regex.hpp"
#include "parser/syntax-information.hpp"

#include "core/conversions.hpp"
#include "parser/expression-compiler-clike.hpp"

const SyntaxTreeGenerator::ArgumentNodeGenerator
    RiscvParser::argumentGeneratorFunction =
        [](const std::string& operand,
           const NodeFactoryCollection& nodeFactories,
           CompileState& state) -> std::unique_ptr<AbstractSyntaxTreeNode> {
  // These checks are performed:
  // * Empty argument? Shouldn't happen, kill the compilation with fire.
  // * First character is a letter? We have replace all constants by now, so it
  // must be a register - or an undefined constant!
  // * If not? Try to compile the expression!
  std::unique_ptr<AbstractSyntaxTreeNode> outputNode;
  if (operand.empty()) {
    outputNode = std::unique_ptr<AbstractSyntaxTreeNode>(nullptr);
  } else if (std::isalpha(operand[0])) {
    outputNode = nodeFactories.createRegisterNode(operand);
  } else {
    // using i32
    int32_t result =
        CLikeExpressionCompilers::CLikeCompilerI32.compile(operand, state);
    outputNode = nodeFactories.createImmediateNode(
        conversions::convert(result,
                             conversions::standardConversions::helper::
                                 twosComplement::toMemoryValueFunction,
                             32));
  }
  return std::move(outputNode);
};

RiscvParser::RiscvParser(const Architecture& architecture,
                         const MemoryAccess& memoryAccess)
: _architecture(architecture), _memoryAccess(memoryAccess) {
  _factory_collection = NodeFactoryCollectionMaker::CreateFor(architecture);
}

FinalRepresentation
RiscvParser::parse(const std::string& text, ParserMode parserMode) {
  IntermediateRepresentator intermediate;
  std::istringstream stream{text};

  // Initialize compile state
  _compile_state.errorList.clear();
  _compile_state.position = CodePosition(0, 0);
  _compile_state.mode = parserMode;


  RiscvRegex line_regex;
  std::vector<std::string> labels, sources, targets;

  for (std::string line; std::getline(stream, line);) {
    _compile_state.position = _compile_state.position.newLine();
    line_regex.matchLine(line);
    if (!line_regex.isValid()) {
      // Add syntax error if line regex doesnt match
      _compile_state.addError("Syntax Error", _compile_state.position);
    } else {
      // Collect labels until next instruction
      if (line_regex.hasLabel()) {
        labels.push_back(line_regex.getLabel());
      }

      if (line_regex.hasInstruction()) {
        // Collect source and target parameters
        for (int i = 0; i < line_regex.getParameterCount(); i++) {
          if (i == 0)
            targets.push_back(line_regex.getParameter(i));
          else
            sources.push_back(line_regex.getParameter(i));
        }

        intermediate.insertCommand(
            IntermediateInstruction{
                LineInterval(_compile_state.position.line()),
                labels,
                line_regex.getInstruction(),
                sources,
                targets},
            _compile_state);

        labels.clear();
        targets.clear();
        sources.clear();
      }
    }
  }

  MemoryAllocator allocator(
      {MemorySectionDefinition("text", 1),
       MemorySectionDefinition("data", _architecture.getWordSize())});
  return intermediate.transform(
      _architecture,
      SyntaxTreeGenerator{_factory_collection, argumentGeneratorFunction},
      allocator,
      _compile_state,
      _memoryAccess);
}

const SyntaxInformation RiscvParser::getSyntaxInformation() {
  SyntaxInformation info;

  // Add instruction regexes
  for (auto instruction : _architecture.getInstructions()) {
    // Matches all instruction mnemonics which don't end with a ':'
    info.addSyntaxRegex("\\b" + instruction.first + "\\b(?!:)",
                        SyntaxInformation::Token::Instruction);
  }

  // Add comment regex
  // Matches everything after a ';'
  info.addSyntaxRegex(";.*", SyntaxInformation::Token::Comment);

  // Add label regex
  // Matches words at the beginning of a line (ignoring whitespaces) which end
  // with a ':'
  info.addSyntaxRegex("^\\s*\\w+:", SyntaxInformation::Token::Label);

  // Add immediate regex
  // Matches arithmetic expressions containing digits, operators, brackets and
  // spaces. Expressions need to start with a digit, an open bracket or an unary
  // operator.
  info.addSyntaxRegex(
      R"(\b[\+\-0-9\(!~][0-9a-fA-Fx\+\-%\*\/\(\)\|\^&=!<>~\t ]*)",
      SyntaxInformation::Token::Immediate);

  // Matches string literals
  info.addSyntaxRegex(R"(".*")", SyntaxInformation::Token::Immediate);
  info.addSyntaxRegex(R"('.*')", SyntaxInformation::Token::Immediate);

  // Add register regexes
  for (UnitInformation unit : _architecture.getUnits()) {
    for (auto reg : unit)
      if (reg.second.hasName())
        // Matches all register names not followed by a ':'
        info.addSyntaxRegex("\\b" + reg.second.getName() + "\\b(?!:)",
                            SyntaxInformation::Token::Register);
  }

  return info;
}
