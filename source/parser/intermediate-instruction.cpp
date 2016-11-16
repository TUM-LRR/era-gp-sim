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

#include "parser/intermediate-instruction.hpp"
#include "arch/common/architecture.hpp"
#include "common/assert.hpp"

void IntermediateInstruction::execute(FinalRepresentation& finalRepresentator,
                                      const SymbolTable& table,
                                      const SyntaxTreeGenerator& generator,
                                      CompileState& state,
                                      MemoryAccess& memoryAccess) {
  // For a machine instruction, it is easy to "execute" it: just insert it into
  // the final form.
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
  table.replaceSymbols(cpy, state);
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

void IntermediateInstruction::enhanceSymbolTable(
    SymbolTable& table, const MemoryAllocator& allocator, CompileState& state) {
  if (_relativeAddress.valid()) {
    _address = allocator.absolutePosition(_relativeAddress);
  } else {
    _address = 0;
  }

  // We insert all our labels.
  for (const auto& i : _labels) {
    table.insertEntry(i, std::to_string(_address), state);
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
  if (!instructionSet.hasInstruction(opcode))
  {
    state.addError("Unknown opcode: " + _name);
    return;
  }

  // For now. Later to be reworked with a bit-level memory allocation?
  std::size_t instructionLength =
      instructionSet[opcode].getLength() / architecture.getByteSize();
  _relativeAddress = allocator["text"].allocateRelative(instructionLength);
}
