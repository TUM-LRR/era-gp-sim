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

#include "intermediate-directive.hpp"
#include "parser/string-parser.hpp"
#include "arch/common/architecture.hpp"
#include "parser/expression-compiler-clike.hpp"
#include "core/conversions.hpp"
#include "core/memory-value.hpp"
#include "core/memory-access.hpp"

template <typename T>
class MemoryDefinitionDirective : public IntermediateDirective {
 public:
  /**
 * \brief Instantiates a new IntermediateDirective with the given arguments.
 * (only for subclass use!)
 * \param lines The line interval the operation occupies.
 * \param labels The vector of labels assigned to the operation.
 * \param name The name of the operation.
 */
  MemoryDefinitionDirective(const LineInterval& lines,
                            const std::vector<std::string>& labels,
                            const std::string& name,
                            std::size_t cellSize,
                            const std::vector<std::string>& values)
  : IntermediateDirective(lines, labels, name)
  , _cellSize(cellSize)
  , _values(values) {
  }

  /**
 * \brief Instantiates a new IntermediateDirective with the given arguments.
 * (only for subclass use!)
 * \param lines The line interval the operation occupies.
 * \param labels The vector of labels assigned to the operation.
 * \param name The name of the operation.
 */
  MemoryDefinitionDirective(const LineInterval& lines,
                            const std::vector<std::string>& labels,
                            const std::string& name,
                            const std::vector<std::string>& values)
  : MemoryDefinitionDirective(lines, labels, name, sizeof(T), values) {
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
    // So, we simply calculate and sum up our arguments.
    std::size_t sizeInBytes = 0;
    for (const auto& i : _values) {
      if (i.empty()) {
        state.addError("Argument is empty.");
      } else if (i.at(0) == '\"') {
        CompileState temporaryState;
        std::vector<T> temporaryData;
        if (StringParser::parseString(i, temporaryData, temporaryState)) {
          sizeInBytes += _cellSize * temporaryData.size();
        }
      } else {
        sizeInBytes += _cellSize;
      }
    }

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
    MemoryValue data(_size);
    std::size_t currentPosition = 0;
    ExpressionCompiler<T> compiler =
        CLikeExpressionCompilers::createCLikeCompiler<T>();
    for (const auto& i : _values) {
      if (!i.empty()) {
        if (i.at(0) == '\"') {
          std::vector<T> returnData;
          if (StringParser::parseString(i, returnData, state)) {  
            for (const auto& j : returnData)
            {
              auto memoryValue = conversions::convert(j,
                             conversions::standardConversions::helper::
                                 twosComplement::toMemoryValueFunction,
                             _byteSize * _cellSize);
              memoryValue.write(data, currentPosition);
              currentPosition += _cellSize;
            }
          }
        } else {
          T returnData = compiler.compile(i, state);
          // Temporary.
          auto memoryValue = conversions::convert(returnData,
                             conversions::standardConversions::helper::
                                 twosComplement::toMemoryValueFunction,
                             _byteSize * _cellSize);
          memoryValue.write(data, currentPosition);
          currentPosition += _cellSize;
        }
      }
    }

    memoryAccess.putMemoryValueAt(_absolutePosition, data);
  }

 private:
  std::size_t _byteSize;
  std::size_t _absolutePosition;
  RelativeMemoryPosition _relativePosition;
  std::size_t _size;
  std::size_t _cellSize;
  std::vector<std::string> _values;
};

#endif /* ERAGPSIM_PARSER_MEMORY_DEFINITION_DIRECTIVE_HPP */
