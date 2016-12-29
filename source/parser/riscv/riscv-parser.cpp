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
#include <regex>
#include <sstream>
#include "arch/common/architecture.hpp"
#include "arch/common/node-factory-collection-maker.hpp"
#include "arch/common/unit-information.hpp"
#include "parser/common/syntax-information.hpp"
#include "parser/independent/intermediate-instruction.hpp"
#include "parser/independent/intermediate-representator.hpp"
#include "parser/riscv/riscv-directive-factory.hpp"
#include "parser/riscv/riscv-regex.hpp"

#include "core/conversions.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/independent/expression-compiler-clike.hpp"
#include "parser/independent/transformation-parameters.hpp"

const SyntaxTreeGenerator::ArgumentNodeGenerator
    RiscvParser::argumentGeneratorFunction = [](
        const PositionedString& operandPositional,
        const SymbolReplacer& replacer,
        const NodeFactoryCollection& nodeFactories,
        CompileErrorList& errors) -> std::shared_ptr<AbstractSyntaxTreeNode> {
  // These checks are performed:
  // * Empty argument? Shouldn't happen, kill the compilation with fire.
  // * First character is a letter? We have replace all constants by now, so
  // it
  // must be a register - or an undefined constant!
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
    // using i32
    int32_t result = CLikeExpressionCompilers::CLikeCompilerI32.compile(
        operandPositional, replacer, errors);
    outputNode = nodeFactories.createImmediateNode(
        conversions::convert(result,
                             conversions::standardConversions::helper::
                                 twosComplement::toMemoryValueFunction,
                             32));
  }
  return outputNode;
};

RiscvParser::RiscvParser(const Architecture& architecture,
                         const MemoryAccess& memoryAccess)
: _architecture(architecture), _memoryAccess(memoryAccess) {
  _factoryCollection = NodeFactoryCollectionMaker::CreateFor(architecture);
}

static CodePositionInterval
uniteStringVector(const PositionedStringVector& positionVector) {
  auto accumulator = CodePositionInterval();
  for (const auto& position : positionVector) {
    accumulator = accumulator.unite(position.positionInterval());
  }
  return accumulator;
}

static CodePositionInterval
getCommandInterval(const PositionedString& instruction,
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

static void readInstruction(const CodePosition& position,
                            const PositionedStringVector& labels,
                            const RiscvParser::RiscvRegex& lineRegex,
                            CompileErrorList& errors,
                            IntermediateRepresentator& intermediate) {
  PositionedStringVector sources, targets;

  auto isDirective = lineRegex.isDirective();
  // Collect source and target parameters
  for (int i = 0; i < lineRegex.getParameterCount(); i++) {
    if (i == 0 && !isDirective)
      targets.push_back(lineRegex.getParameter(i));
    else
      sources.push_back(lineRegex.getParameter(i));
  }

  auto instruction = lineRegex.getInstruction();

  auto interval = getCommandInterval(instruction, labels, sources, targets);
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

static void readText(const std::string& text,
                     CompileErrorList& errors,
                     IntermediateRepresentator& intermediate) {
  std::istringstream stream(text);
  CodePosition position;
  RiscvParser::RiscvRegex lineRegex;
  PositionedStringVector labels;

  for (std::string line; std::getline(stream, line);) {
    position = position.newLine();
    lineRegex.matchLine(line, position.line(), errors);
    if (!lineRegex.isValid()) {
      // Add syntax error if line regex doesnt match
      errors.pushError(CodePositionInterval(position, position >> 1),
                       "Syntax Error");
    } else {
      // Collect labels until next instruction
      if (lineRegex.hasLabel()) {
        labels.push_back(lineRegex.getLabel());
      }

      if (lineRegex.hasInstruction()) {
        readInstruction(position, labels, lineRegex, errors, intermediate);

        labels.clear();
      }
    }
  }
}

FinalRepresentation RiscvParser::parse(const std::string& text) {
  IntermediateRepresentator intermediate;
  CompileErrorList errors;

  // We append all built-in macros to our text.
  auto extendedText = text + "\n" + _architecture.getBuiltinMacros();

  readText(extendedText, errors, intermediate);

  auto byteAlignment =
      _architecture.getWordSize() / _architecture.getByteSize();
  MemoryAllocator allocator({MemorySectionDefinition("text", 1),
                             MemorySectionDefinition("data", byteAlignment)});

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

  // Add instruction regexes
  for (auto instruction : _architecture.getInstructions()) {
    // Matches all instruction mnemonics which don't end with a ':'
    info.addSyntaxRegex("\\b" + instruction.first + "\\b(?!:)",
                        SyntaxInformation::Token::Instruction);
  }

  // Add directive regexes
  for (auto directive : RiscVDirectiveFactory::mapping) {
    // Matches all directive mnemonics starting with a '.' which don't end with
    // a ':'
    info.addSyntaxRegex("\\." + directive.first + "\\b(?!:)",
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
