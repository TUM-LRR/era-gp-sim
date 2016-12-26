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

#include "parser/independent/intermediate-operation.hpp"

#include "common/assert.hpp"
#include "parser/common/code-position-interval.hpp"
#include "parser/common/final-representation.hpp"
#include "parser/independent/enhance-symbol-table-immutable-arguments.hpp"
#include "parser/independent/execute-immutable-arguments.hpp"
#include "parser/independent/memory-allocator.hpp"
#include "parser/independent/memory-section-definition.hpp"
#include "parser/independent/preprocessing-immutable-arguments.hpp"
#include "parser/independent/relative-memory-position.hpp"
#include "parser/independent/symbol-graph.hpp"
#include "parser/independent/symbol-replacer.hpp"
#include "parser/independent/syntax-tree-generator.hpp"

IntermediateOperation::IntermediateOperation(
    const CodePositionInterval& positionInterval,
    const std::vector<PositionedString>& labels,
    const PositionedString& name)
: _positionInterval(positionInterval), _labels(labels), _name(name) {
}

void IntermediateOperation::execute(const ExecuteImmutableArguments& immutable,
                                    CompileErrorList& errors,
                                    FinalCommandVector& commandOutput,
                                    MemoryAccess& memoryAccess) {
}

void IntermediateOperation::enhanceSymbolTable(
    const EnhanceSymbolTableImmutableArguments& immutable,
    CompileErrorList& errors,
    SymbolGraph& graph) {
}

void IntermediateOperation::allocateMemory(
    const PreprocessingImmutableArguments& immutable,
    CompileErrorList& errors,
    MemoryAllocator& allocator,
    SectionTracker& tracker) {
}

void IntermediateOperation::precompile(
    const PreprocessingImmutableArguments& immutable,
    CompileErrorList& errors,
    MacroDirectiveTable& macroTable) {
}

bool IntermediateOperation::shouldInsert() const {
  return true;
}

TargetSelector IntermediateOperation::newTarget() const {
  return TargetSelector::KEEP;
}

void IntermediateOperation::insert(IntermediateOperationPointer pointer) {
  // If this happens, something has gone wrong in our programming.
  assert::that(false);
}

void IntermediateOperation::insertIntoArguments(const PositionedString& name,
                                                const PositionedString& value) {
  // Nothing to do here.
}

IntermediateOperationPointer IntermediateOperation::clone() {
  return nullptr;
}

const CodePositionInterval& IntermediateOperation::positionInterval() const
    noexcept {
  return _positionInterval;
}

const std::vector<PositionedString>& IntermediateOperation::labels() const
    noexcept {
  return _labels;
}

const PositionedString& IntermediateOperation::name() const noexcept {
  return _name;
}

std::string IntermediateOperation::toString() const {
  return _name.string() + "\n";
}

IntermediateOperation::Type IntermediateOperation::getType() const {
  return Type::OTHER;
}
