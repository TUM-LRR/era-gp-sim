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

#include "parser/intermediate-operation.hpp"

#include "common/assert.hpp"
#include "parser/final-representation.hpp"
#include "parser/memory-allocator.hpp"
#include "parser/symbol-table.hpp"
#include "parser/syntax-tree-generator.hpp"

IntermediateOperation::IntermediateOperation(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name)
: _lines(lines), _labels(labels), _name(name) {
}

void IntermediateOperation::allocateMemory(const Architecture& architecture,
                                           MemoryAllocator& allocator,
                                           CompileState& state) {
}

void IntermediateOperation::enhanceSymbolTable(SymbolTable& table,
                                               const MemoryAllocator& allocator,
                                               CompileState& state) {
}

bool IntermediateOperation::shouldInsert() const {
  return true;
}

TargetSelector IntermediateOperation::newTarget() const {
  return TargetSelector::KEEP;
}

IntermediateExecutionTime IntermediateOperation::executionTime() const {
  return IntermediateExecutionTime::AFTER_ALLOCATION;
}

void IntermediateOperation::insert(IntermediateOperationPointer pointer) {
  // If this happens, something has gone wrong in our programming.
  assert::that(false);
}

void IntermediateOperation::insertIntoArguments(const std::string& name,
                                                const std::string& value) {
  // Nothing to do here.
}

IntermediateOperationPointer IntermediateOperation::clone() {
  return nullptr;
}

const LineInterval& IntermediateOperation::lines() const {
  return _lines;
}

const std::vector<std::string>& IntermediateOperation::labels() const {
  return _labels;
}

const std::string& IntermediateOperation::name() const {
  return _name;
}
