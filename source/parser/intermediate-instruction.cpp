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
#include "common/assert.hpp"
#include "core/conversions.hpp"
#include "core/memory-access.hpp"
#include "parser/final-representation.hpp"
#include "parser/intermediate-instruction.hpp"
#include "parser/macro-directive.hpp"
#include "parser/symbol-table.hpp"
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

void IntermediateInstruction::execute(FinalRepresentation& finalRepresentator,
                                      const SymbolTable& table,
                                      const SyntaxTreeGenerator& generator,
                                      CompileState& state,
                                      MemoryAccess& memoryAccess) {
  // For a machine instruction, it is easy to "execute" it: just insert it
  // into the final form.
  finalRepresentator.commandList.push_back(
      compileInstruction(table, generator, state, memoryAccess));
}

std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>
IntermediateInstruction::compileArgumentVector(
    const std::vector<std::string>& vector,
    const SymbolTable& table,
    const SyntaxTreeGenerator& generator,
    CompileState& state) {
  // First of all, we insert all constants. Then, we convert every single one of
  // them to a syntax tree node.
  std::vector<std::string> cpy(vector);
  table.replaceSymbols(
      cpy,
      state,
      [&, generator](const std::string& replace,
                     SymbolTable::SymbolType type) -> std::string {
        // When inserting a label, we might transform its value into a relative
        // one (depends on the instruction)
        // We use NodeFactoryCollection::labelToImmediate which translates the
        // value if necessary
        if (type != SymbolTable::SymbolType::LABEL) {
          return replace;
        } else {
          MemoryValue labelValue = conversions::convert<size_t>(
              std::stoul(replace), sizeof(size_t) * 8);
          MemoryValue instructionAdress = conversions::convert<size_t>(
              _relativeAddress.offset, sizeof(size_t) * 8);
          MemoryValue relativeAdress =
              generator.getNodeFactories().labelToImmediate(
                  labelValue, _name, instructionAdress);
          return relativeAdress.toHexString(true, true);
        }
      });
  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> output;
  output.reserve(cpy.size());
  for (const auto& i : cpy) {
    std::unique_ptr<AbstractSyntaxTreeNode> argument{
        generator.transformOperand(i, state)};

    // Only add argument node if creation was successfull.
    // Otherwise AbstractSyntaxTreeNode::validate() segfaults.
    if (argument) {
      output.emplace_back(std::move(argument));
    }
  }
  return output;
}

FinalCommand IntermediateInstruction::compileInstruction(
    const SymbolTable& table,
    const SyntaxTreeGenerator& generator,
    CompileState& state,
    MemoryAccess& memoryAccess) {
  // We replace all occurenced in target in source (using a copy of them).
  auto srcCompiled = compileArgumentVector(_sources, table, generator, state);
  auto trgCompiled = compileArgumentVector(_targets, table, generator, state);
  FinalCommand result;
  result.node = std::move(generator.transformCommand(
      _name, srcCompiled, trgCompiled, state, memoryAccess));
  result.position = _lines;
  result.address = _address;
  return result;
}

MemoryAddress IntermediateInstruction::address() const {
  return _address;
}

void IntermediateInstruction::enhanceSymbolTable(
    SymbolTable& table, const MemoryAllocator& allocator, CompileState& state) {
  if (_relativeAddress.valid()) {
    _address = allocator.absolutePosition(_relativeAddress);
  } else {
    _address = 0;
  }

  // We insert all our labels.
  for (const auto& i : _labels) {
    table.insertEntry(
        i, std::to_string(_address), state, SymbolTable::SymbolType::LABEL);
  }
}

void IntermediateInstruction::allocateMemory(const Architecture& architecture,
                                             MemoryAllocator& allocator,
                                             CompileState& state) {
  if (state.section != "text") {
    state.addError("Tried to define an instruction in not the text section.");
    return;
  }

  const auto& instructionSet = architecture.getInstructions();

  // toLower as long as not fixed in instruction set.
  auto opcode = Utility::toLower(_name);
  if (!instructionSet.hasInstruction(opcode)) {
    // state.addError("Unknown opcode: " + _name);
    return;
  }

  // For now. Later to be reworked with a bit-level memory allocation?
  std::size_t instructionLength =
      instructionSet[opcode].getLength() / architecture.getByteSize();
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
