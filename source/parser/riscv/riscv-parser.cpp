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

#include "parser/riscv/riscv-parser.hpp"

#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "arch/common/architecture.hpp"
#include "arch/common/node-factory-collection-maker.hpp"
#include "arch/common/unit-information.hpp"
#include "common/string-conversions.hpp"
#include "core/conversions.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/common/syntax-information.hpp"
#include "parser/independent/execute-immutable-arguments.hpp"
#include "parser/independent/expression-compiler-clike.hpp"
#include "parser/independent/intermediate-instruction.hpp"
#include "parser/independent/intermediate-representator.hpp"
#include "parser/independent/relative-memory-position.hpp"
#include "parser/independent/transformation-parameters.hpp"
#include "parser/riscv/riscv-directive-factory.hpp"
#include "parser/riscv/riscv-regex.hpp"

const SyntaxTreeGenerator::ArgumentNodeGenerator
    RiscvParser::argumentGeneratorFunction = [](
        const PositionedString& operandPositional,
        const SymbolReplacer& replacer,
        const NodeFactoryCollection& nodeFactories,
        CompileErrorList& errors) -> std::shared_ptr<AbstractSyntaxTreeNode> {
  // These checks are performed:
  // * Empty argument? Shouldn't happen, kill the compilation with fire.
  // * First character is a letter? We have replace all constants by now, so
  // it must be a register - or an undefined constant!
  // * If not? Try to compile the expression!
  std::shared_ptr<AbstractSyntaxTreeNode> outputNode;
  PositionedString operandReplaced =
      replacer.replace(operandPositional, errors);
  const auto& operand = operandReplaced.string();
  if (operand.empty()) {
    outputNode = std::shared_ptr<AbstractSyntaxTreeNode>(nullptr);
  } else if (std::isalpha(operand[0])) {
    outputNode = nodeFactories.createRegisterNode(operand);
  } else if (operand[0] == '\"') {
    // Data nodes are mainly for meta instructions.
    std::vector<char> outString;
    StringParser::parseString(operandPositional, errors, outString);
    std::string asString(outString.begin(), outString.end());
    outputNode = nodeFactories.createDataNode(asString);
  } else {
    // using i64
    int64_t result = CLikeExpressionCompilers::CLikeCompilerI64.compile(
        operandPositional, replacer, errors);
    outputNode = nodeFactories.createImmediateNode(
        conversions::convert(result,
                             conversions::standardConversions::helper::
                                 twosComplement::toMemoryValueFunction,
                             64));
  }
  return outputNode;
};

RiscvParser::RiscvParser(const Architecture& architecture,
                         const MemoryAccess& memoryAccess)
: _architecture(architecture), _memoryAccess(memoryAccess) {
  _factoryCollection = NodeFactoryCollectionMaker::CreateFor(architecture);
}
namespace {
CodePositionInterval
uniteStringVector(const PositionedStringVector& positionVector) {
  auto accumulator = CodePositionInterval();
  for (const auto& position : positionVector) {
    accumulator = accumulator.unite(position.positionInterval());
  }
  return accumulator;
}

CodePositionInterval getCommandInterval(const PositionedString& instruction,
                                        const PositionedStringVector& labels,
                                        const PositionedStringVector& sources,
                                        const PositionedStringVector& targets) {
  auto labelsUnited = uniteStringVector(labels);
  auto sourcesUnited = uniteStringVector(sources);
  auto targetsUnited = uniteStringVector(targets);
  return instruction.positionInterval()
      .unite(labelsUnited)
      .unite(sourcesUnited)
      .unite(targetsUnited);
}

void readInstruction(const CodePosition& position,
                     const PositionedStringVector& labels,
                     const RiscvParser::RiscvRegex& lineRegex,
                     CompileErrorList& errors,
                     IntermediateRepresentator& intermediate,
                     bool hidden) {
  PositionedStringVector sources, targets;

  auto isDirective = lineRegex.isDirective();
  // Collect source and target parameters
  for (int i = 0; i < lineRegex.getParameterCount(); i++) {
    if (i == 0 && !isDirective)
      targets.emplace_back(lineRegex.getParameter(i));
    else
      sources.emplace_back(lineRegex.getParameter(i));
  }

  auto instruction = lineRegex.getInstruction();

  auto interval = getCommandInterval(instruction, labels, sources, targets);

  if (hidden) interval = {};

  if (isDirective) {
    RiscVDirectiveFactory::create(
        interval, labels, instruction, sources, intermediate, errors);
  } else {
    intermediate.insertCommand(
        IntermediateInstruction(
            interval, labels, instruction, sources, targets),
        errors);
  }
}

void readText(const std::string& text,
              CompileErrorList& errors,
              IntermediateRepresentator& intermediate,
              bool hidden = false) {
  std::istringstream stream(text);
  CodePosition position;
  RiscvParser::RiscvRegex lineRegex;
  PositionedStringVector labels;

  for (std::string line; std::getline(stream, line);) {
    position = position.newLine();
    lineRegex.matchLine(line, position.line(), errors);
    if (!lineRegex.isValid()) {
      // Add syntax error if line regex doesnt match
      errors.pushError(CodePositionInterval(position, position),
                       "Syntax Error");
    } else {
      // Collect labels until next instruction
      if (lineRegex.hasLabel()) {
        labels.emplace_back(lineRegex.getLabel());
      }

      if (lineRegex.hasInstruction()) {
        readInstruction(
            position, labels, lineRegex, errors, intermediate, hidden);

        labels.clear();
      }
    }
  }
}
}  // namespace

FinalRepresentation RiscvParser::parse(const std::string& text) {
  IntermediateRepresentator intermediate;
  CompileErrorList errors;

  // First we parse the builtin macros
  readText(_architecture.getBuiltinMacros(), errors, intermediate, true);

  // Then we parse the user text
  readText(text, errors, intermediate);

  auto byteAlignment =
      _architecture.getWordSize() / _architecture.getByteSize();

  auto riscvInstructionSize = 4u;

  MemoryAllocator allocator(
      {MemorySectionDefinition("data", byteAlignment, 1),
       MemorySectionDefinition("text", riscvInstructionSize, 1)});

  TransformationParameters parameters(
      _architecture,
      allocator,
      SyntaxTreeGenerator(_factoryCollection, argumentGeneratorFunction));

  auto intermediateOutput =
      intermediate.transform(parameters, errors, _memoryAccess);

  return intermediateOutput;
}

const SyntaxInformation RiscvParser::getSyntaxInformation() {
  SyntaxInformation info;

  // Matches single words after a ':'' or line-start that does not end with ':'
  info.addSyntaxRegex(R"((?<=^|:) *[a-zA-Z.]+\b(?!:))",
                      SyntaxInformation::Token::Instruction);

  // Add comment regex
  // Matches everything after a ';'
  info.addSyntaxRegex(";.*", SyntaxInformation::Token::Comment);

  // Add label regex
  // Matches words starting with a letter or '_'
  info.addSyntaxRegex("\\b[a-zA-Z_]\\w*", SyntaxInformation::Token::Label);

  // Add immediate regex
  // Matches expressions operators and brackets
  info.addSyntaxRegex(
      R"([\+\-%\*\/\(\)\|\^&=!<>~_]+)", SyntaxInformation::Token::Immediate);
  // Matches Numbers
  info.addSyntaxRegex(
      R"(\b[0-9]+\b)", SyntaxInformation::Token::Immediate);
  info.addSyntaxRegex(
      R"(\b0b[01]+\b)", SyntaxInformation::Token::Immediate);
  info.addSyntaxRegex(
      R"(\b0x[0-9a-fA-F]+\b)", SyntaxInformation::Token::Immediate);

  // Matches string literals
  info.addSyntaxRegex(R"(".*")", SyntaxInformation::Token::Immediate);
  info.addSyntaxRegex(R"('.*')", SyntaxInformation::Token::Immediate);

  // Add register regexes
  for (UnitInformation unit : _architecture.getUnits()) {
    for (auto reg : unit) {
      if (reg.second.hasName()) {
        // Matches all register names not followed by a ':'
        info.addSyntaxRegex("\\b" + reg.second.getName() + "\\b(?!:)",
                            SyntaxInformation::Token::Register);
      }
    }
  }

  return info;
}
