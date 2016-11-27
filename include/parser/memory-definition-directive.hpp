/*
* C++ Assembler Interpreter
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

#ifndef ERAGPSIM_PARSER_MEMORY_DEFINITION_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_MEMORY_DEFINITION_DIRECTIVE_HPP

#include <functional>
#include "arch/common/architecture.hpp"
#include "core/conversions.hpp"
#include "core/memory-access.hpp"
#include "core/memory-value.hpp"
#include "intermediate-directive.hpp"
#include "parser/expression-compiler-clike.hpp"
#include "parser/string-parser.hpp"

/**
 * \brief A directive to reserve memory and writes specified data to it.
 * \tparam T The number type used for parsing.
 */
template <typename T>
class MemoryDefinitionDirective : public IntermediateDirective {
 public:
  using MemoryStorageFunction = std::function<void(T, std::size_t)>;
  using ProcessValuesFunction =
      std::function<std::size_t(const std::vector<std::string>&,
                                std::size_t,
                                CompileState&,
                                const MemoryStorageFunction&)>;

  /**
 * \brief Instantiates a new MemoryDefinitionDirective with the given arguments.
 * \param lines The line interval the operation occupies.
 * \param labels The vector of labels assigned to the operation.
 * \param name The name of the operation.
 * \param cellSize The size of the unit how much each argument should occupy at
 * space.
 * \param values The argument vector.
 * \param processValues A function to parse the arguments.
 */
  MemoryDefinitionDirective(const LineInterval& lines,
                            const std::vector<std::string>& labels,
                            const std::string& name,
                            std::size_t cellSize,
                            const std::vector<std::string>& values,
                            const ProcessValuesFunction& processValues)
  : IntermediateDirective(lines, labels, name)
  , _cellSize(cellSize)
  , _values(values)
  , _processValues(processValues) {
  }

  /**
 * \brief Instantiates a new MemoryDefinitionDirective with the given arguments.
 * \param lines The line interval the operation occupies.
 * \param labels The vector of labels assigned to the operation.
 * \param name The name of the operation.
 * \param values The argument vector.
 * \param processValues A function to parse the arguments.
 */
  MemoryDefinitionDirective(const LineInterval& lines,
                            const std::vector<std::string>& labels,
                            const std::string& name,
                            const std::vector<std::string>& values,
                            const ProcessValuesFunction& processValues)
  : MemoryDefinitionDirective(
        lines, labels, name, sizeof(T), values, processValues) {
  }

  /**
   * \brief Reserves (not writes!) memory for the operation (if needed).
   * \param architecture The architecture for information about the memory
   * format.
   * \param allocator The allocator to reserve memory.
   * \param state The CompileState to log possible errors.
   */
  virtual void allocateMemory(const Architecture& architecture,
                              MemoryAllocator& allocator,
                              CompileState& state) {
    if (_values.empty()) {
      state.addError("Arguments missing here.");
    }

    // So, we simply calculate and sum up our arguments.
    // Let's hope, the compiler optimizes this...
    CompileState temporaryState;
    std::size_t sizeInBytes = _processValues(
        _values, _cellSize, temporaryState, [](T value, std::size_t position) {
        });

    // Next, we got to allocate our memory.
    _relativePosition = allocator[state.section].allocateRelative(sizeInBytes);

    // The bit size is store for further usage.
    _size = sizeInBytes * architecture.getByteSize();

    _byteSize = architecture.getByteSize();
  }

  /**
   * \brief Enhances the symbol table by the labels of the operation.
   * \param table The SymbolTable to insert into.
   * \param allocator The MemoryAllocator to get the memory positions from.
   * \param state The CompileState to log possible errors.
   */
  virtual void enhanceSymbolTable(SymbolTable& table,
                                  const MemoryAllocator& allocator,
                                  CompileState& state) {
    _absolutePosition = allocator.absolutePosition(_relativePosition);
    for (const auto& i : _labels) {
      table.insertEntry(i, std::to_string(_absolutePosition), state);
    }
  }

  /**
   * \brief Executes the given operation (somehow).
   * \param finalRepresentator The FinalRepresentation for possible output.
   * \param table The SymbolTable for possible replacements.
   * \param generator The generator to transform the instructions.
   * \param state The CompileState to log possible errors.
   * \param memoryAccess The MemoryAccess for verifying instructions or
   * reserving data.
   */
  virtual void execute(FinalRepresentation& finalRepresentator,
                       const SymbolTable& table,
                       const SyntaxTreeGenerator& generator,
                       CompileState& state,
                       MemoryAccess& memoryAccess) {
    if (_size > 0) {
      // We first of all create our memory value locally.
      MemoryValue data(_size);

      // Then we write to it.
      _processValues(
          _values, _cellSize, state, [&](T value, std::size_t position) {
            // For now. Later to be replaced by the real enum of the arch,
            // maybe...
            auto memoryValue =
                conversions::convert(value,
                                     conversions::standardConversions::helper::
                                         twosComplement::toMemoryValueFunction,
                                     _byteSize * _cellSize);

            // Once converted, we take down the value.
            data.write(memoryValue, position * _byteSize);
          });

      // Then, let's do a (probably also here) expensive memory call.
      memoryAccess.putMemoryValueAt(_absolutePosition, data);
    } else {
      state.addError("Nothing to reserve with memory definition.");
    }
  }

 private:
  ProcessValuesFunction _processValues;
  std::size_t _byteSize;
  std::size_t _absolutePosition;
  RelativeMemoryPosition _relativePosition;
  std::size_t _size;
  std::size_t _cellSize;
  std::vector<std::string> _values;
};

#endif /* ERAGPSIM_PARSER_MEMORY_DEFINITION_DIRECTIVE_HPP */
