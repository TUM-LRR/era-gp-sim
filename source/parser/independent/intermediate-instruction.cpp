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
#include "common/utility.hpp"
#include "core/conversions.hpp"
#include "core/memory-access.hpp"
#include "parser/common/code-position-interval.hpp"
#include "parser/common/code-position.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/common/final-representation.hpp"
#include "parser/independent/enhance-symbol-table-immutable-arguments.hpp"
#include "parser/independent/execute-immutable-arguments.hpp"
#include "parser/independent/intermediate-instruction.hpp"
#include "parser/independent/macro-directive.hpp"
#include "parser/independent/positioned-string.hpp"
#include "parser/independent/preprocessing-immutable-arguments.hpp"
#include "parser/independent/section-tracker.hpp"
#include "parser/independent/symbol-graph.hpp"
#include "parser/independent/symbol-replacer.hpp"
#include "parser/independent/syntax-tree-generator.hpp"

using size_t = std::size_t;

IntermediateInstruction::IntermediateInstruction(
    const CodePositionInterval& positionInterval,
    const std::vector<PositionedString>& labels,
    const PositionedString& name,
    const std::vector<PositionedString>& sources,
    const std::vector<PositionedString>& targets)
: IntermediateOperation(positionInterval, labels, name)
, _sources(sources)
, _targets(targets) {
}

void IntermediateInstruction::execute(
    const ExecuteImmutableArguments& immutable,
    CompileErrorList& errors,
    FinalCommandVector& commandOutput,
    MemoryAccess& memoryAccess) {
  // For a machine instruction, it is easy to "execute" it: just insert it
  // into the final form.
  auto compiled = compileInstruction(immutable, errors, memoryAccess);
  if (compiled.node()) {
    commandOutput.push_back(compiled);
  }
}

std::vector<std::shared_ptr<AbstractSyntaxTreeNode>>
IntermediateInstruction::compileArgumentVector(
    const std::vector<PositionedString>& vector,
    const ExecuteImmutableArguments& immutable,
    CompileErrorList& errors,
    MemoryAccess& memoryAccess) {
  std::vector<std::shared_ptr<AbstractSyntaxTreeNode>> output;

  SymbolReplacer::DynamicReplacer labelReplacerFunction = [&](
      const Symbol& symbol) {
    // This might not be the biggest type, but we got to consider that the
    // memory also works on size_t.
    size_t byteBitSize =
        sizeof(size_t) * immutable.architecture().getByteSize();
    auto labelValue = conversions::convert<size_t>(
        std::stoul(symbol.value().string()), byteBitSize);
    auto instructionAdress =
        conversions::convert<size_t>(_relativeAddress.offset(), byteBitSize);
    auto relativeAdress =
        immutable.generator().getNodeFactories().labelToImmediate(
            labelValue, name().string(), instructionAdress);
    return relativeAdress.toHexString(true, true);
  };

  auto replacer = SymbolReplacer(immutable.replacer(), labelReplacerFunction);

  output.reserve(vector.size());
  for (const auto& operand : vector) {
    auto transformed =
        immutable.generator().transformOperand(operand, replacer, errors);

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
    CompileErrorList& errors,
    MemoryAccess& memoryAccess) {
  // We replace all occurenced in target in source (using a copy of them).
  auto srcCompiled =
      compileArgumentVector(_sources, immutable, errors, memoryAccess);
  auto trgCompiled =
      compileArgumentVector(_targets, immutable, errors, memoryAccess);

  auto node = immutable.generator().transformCommand(
      name(), errors, srcCompiled, trgCompiled, memoryAccess);
  auto result = FinalCommand(node, positionInterval(), _address);
  return result;
}

MemoryAddress IntermediateInstruction::address() const noexcept {
  return _address;
}

void IntermediateInstruction::enhanceSymbolTable(
    const EnhanceSymbolTableImmutableArguments& immutable,
    CompileErrorList& errors,
    SymbolGraph& graph) {
  if (_relativeAddress.valid()) {
    _address = immutable.allocator().absolutePosition(_relativeAddress);
  } else {
    _address = 0;
  }

  // We insert all our labels.
  for (const auto& label : labels()) {
    graph.addNode(Symbol(
        label,
        PositionedString(std::to_string(_address), name().positionInterval()),
        SymbolBehavior::DYNAMIC));
  }
}

void IntermediateInstruction::allocateMemory(
    const PreprocessingImmutableArguments& immutable,
    CompileErrorList& errors,
    MemoryAllocator& allocator,
    SectionTracker& tracker) {
  if (tracker.section() != "text") {
    errors.pushError(name().positionInterval(),
                     "Tried to define an instruction in not the text section.");
    return;
  }

  const auto& architecture = immutable.architecture();
  const auto& nameString = name().string();
  const auto& instructionSet = immutable.architecture().getInstructions();

  // toLower as long as not fixed in instruction set.
  if (!instructionSet.hasInstruction(nameString)) {
    // If we'd record an error here, we would do that twice in total, so no.
    return;
  }

  // For now. Later to be reworked with a bit-level memory allocation?
  auto instructionLength =
      instructionSet[nameString].getLength() / architecture.getByteSize();
  _relativeAddress = allocator["text"].allocateRelative(instructionLength);
}

static bool isWordCharacter(char c) {
  return (c == '_' || std::isalpha(c) || std::isdigit(c));
}

static void replaceInVector(std::vector<PositionedString>& vector,
                            const PositionedString& name,
                            const PositionedString& value) {
  auto search = '\\' + name.string();
  for (auto i : Utility::range<size_t>(0, vector.size())) {
    auto str = vector[i].string();
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
      str.replace(pos, search.length(), value.string());
      pos += value.string().length();
    }
    vector[i] = PositionedString(str, vector[i].positionInterval());
  }
}

void IntermediateInstruction::insertIntoArguments(
    const PositionedString& name, const PositionedString& value) {
  replaceInVector(_sources, name, value);
  replaceInVector(_targets, name, value);
}

std::string IntermediateInstruction::toString() const {
  auto str = name().string();

  // Append targets
  for (auto i : Utility::range<size_t>(0, _targets.size())) {
    if (i != 0) {
      str += ", ";
    } else {
      str += " ";
    }
    str += _targets[i].string();
  }

  // Append sources
  for (auto i : Utility::range<size_t>(0, _sources.size())) {
    if (i == 0 && _targets.size() == 0) {
      str += " ";
    } else {
      str += ", ";
    }
    str += _sources[i].string();
  }

  str += "\n";
  return str;
}

IntermediateOperationPointer IntermediateInstruction::clone() {
  return IntermediateOperationPointer{new IntermediateInstruction{*this}};
}

std::vector<PositionedString> IntermediateInstruction::getArgsVector() const {
  std::vector<PositionedString> args;
  args.reserve(_sources.size() + _targets.size());
  args.insert(args.end(), _targets.begin(), _targets.end());
  args.insert(args.end(), _sources.begin(), _sources.end());
  return args;
}

IntermediateOperation::Type IntermediateInstruction::getType() const {
  return Type::INSTRUCTION;
}

const std::vector<PositionedString>& IntermediateInstruction::sources() const
    noexcept {
  return _sources;
}
const std::vector<PositionedString>& IntermediateInstruction::targets() const
    noexcept {
  return _targets;
}
