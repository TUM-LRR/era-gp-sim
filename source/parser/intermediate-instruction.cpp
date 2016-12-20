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

#include <string>
#include <vector>

#include "arch/common/architecture.hpp"
#include "arch/common/architecture.hpp"
#include "common/assert.hpp"
#include "core/conversions.hpp"
#include "core/memory-access.hpp"
#include "parser/code-position-interval.hpp"
#include "parser/code-position.hpp"
#include "parser/compile-error-annotator.hpp"
#include "parser/final-representation.hpp"
#include "parser/intermediate-instruction.hpp"
#include "parser/intermediate-parameters.hpp"
#include "parser/macro-directive.hpp"
#include "parser/section-tracker.hpp"
#include "parser/symbol-graph.hpp"
#include "parser/symbol-replacer.hpp"
#include "parser/syntax-tree-generator.hpp"

IntermediateInstruction::IntermediateInstruction(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    const std::vector<std::string>& sources,
    const std::vector<std::string>& targets)
: IntermediateOperation(lines, labels, name)
, _sources(sources)
, _targets(targets) {
}

void IntermediateInstruction::execute(
    const ExecuteImmutableArguments& immutable,
    const CompileErrorAnnotator& annotator,
    FinalRepresentation& finalRepresentator,
    MemoryAccess& memoryAccess) {
  // For a machine instruction, it is easy to "execute" it: just insert it
  // into the final form.
  finalRepresentator.commandList.push_back(
      compileInstruction(immutable, annotator, memoryAccess));
}

std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>
IntermediateInstruction::compileArgumentVector(
    const std::vector<std::string>& vector,
    const ExecuteImmutableArguments& immutable,
    const CompileErrorAnnotator& annotator,
    MemoryAccess& memoryAccess) {
  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> output;
  output.reserve(vector.size());
  for (const auto& operand : vector) {
    auto replaced = immutable.replacer().replace(operand);
    auto transformed =
        immutable.generator().transformOperand(replaced, annotator);

    // Only add argument node if creation was successful.
    // Otherwise AbstractSyntaxTreeNode::validate() segfaults.
    if (transformed) {
      output.emplace_back(std::move(transformed));
    }
  }
  return output;
}

FinalCommand IntermediateInstruction::compileInstruction(
    const ExecuteImmutableArguments& immutable,
    const CompileErrorAnnotator& annotator,
    MemoryAccess& memoryAccess) {
  // We replace all occurenced in target in source (using a copy of them).
  auto srcCompiled =
      compileArgumentVector(_sources, immutable, annotator, memoryAccess);
  auto trgCompiled =
      compileArgumentVector(_targets, immutable, annotator, memoryAccess);
  // TODO: Rework FinalCommand
  FinalCommand result;
  result.node = std::move(immutable.generator().transformCommand(
      _name, annotator, srcCompiled, trgCompiled, memoryAccess));
  result.position = _lines;
  result.address = _address;
  return result;
}

MemoryAddress IntermediateInstruction::address() const {
  return _address;
}

void IntermediateInstruction::enhanceSymbolTable(
    const EnhanceSymbolTableImmutableArguments& immutable,
    const CompileErrorAnnotator& annotator,
    SymbolGraph& graph) {
  if (_relativeAddress.valid()) {
    _address = immutable.allocator().absolutePosition(_relativeAddress);
  } else {
    _address = 0;
  }

  // We insert all our labels.
  for (const auto& label : _labels) {
    graph.addNode(
        Symbol(label,
               std::to_string(_address),
               CodePositionInterval(CodePosition(0), CodePosition(0)) /*TODO*/,
               SymbolBehavior::DYNAMIC));
  }
}

void IntermediateInstruction::allocateMemory(
    const PreprocessingImmutableArguments& immutable,
    const CompileErrorAnnotator& annotator,
    MemoryAllocator& allocator,
    SectionTracker& tracker) {
  if (tracker.section() != "text") {
    annotator.add("Tried to define an instruction in not the text section.");
    return;
  }

  const auto& instructionSet = immutable.architecture().getInstructions();

  // toLower as long as not fixed in instruction set.
  if (!instructionSet.hasInstruction(_name)) {
    // state.addError("Unknown opcode: " + _name);
    return;
  }

  // For now. Later to be reworked with a bit-level memory allocation?
  auto instructionLength = instructionSet[_name].getLength() /
                           immutable.architecture().getByteSize();
  _relativeAddress = allocator["text"].allocateRelative(instructionLength);
}

static bool isWordCharacter(char c) {
  return (c == '_' || std::isalpha(c) || std::isdigit(c));
}

static void replaceInVector(std::vector<std::string>& vector,
                            const std::string& name,
                            const std::string& value) {
  std::string search = '\\' + name;
  for (int i = 0; i < vector.size(); i++) {
    std::string& str{vector[i]};
    // Replace all occurences of '\\name' that are followed by a non-word char.
    std::string::size_type pos = 0;
    while ((pos = str.find(search, pos)) != std::string::npos) {
      // If search result is followed by a word character, skip it and continue.
      if (pos + search.length() < str.length() &&
          isWordCharacter(str.at(pos + search.length()))) {
        pos += search.length();
        continue;
      }

      // Insert value at position and skip it
      str.replace(pos, search.length(), value);
      pos += value.length();
    }
  }
}

void IntermediateInstruction::insertIntoArguments(const std::string& name,
                                                  const std::string& value) {
  replaceInVector(_sources, name, value);
  replaceInVector(_targets, name, value);
}

IntermediateOperationPointer IntermediateInstruction::clone() {
  return IntermediateOperationPointer{new IntermediateInstruction{*this}};
}

std::vector<std::string> IntermediateInstruction::getArgsVector() const {
  std::vector<std::string> args;
  args.reserve(_sources.size() + _targets.size());
  args.insert(args.end(), _targets.begin(), _targets.end());
  args.insert(args.end(), _sources.begin(), _sources.end());
  return args;
}
