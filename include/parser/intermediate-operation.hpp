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

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_OPERATION_HPP_
#define ERAGPSIM_PARSER_INTERMEDIATE_OPERATION_HPP_

#include <string>
#include <vector>
#include "parser/final-representation.hpp"
#include "parser/symbol-table.hpp"
#include "parser/syntax-tree-generator.hpp"

struct CompileState;

/**
 * \brief Representa an interval of lines, denoted by an upper and lower line
 * bound.
 */
using LineInterval = std::pair<unsigned int, unsigned int>;

/**
 * \brief A memory address substitute as long as we do not have one.
 */
using DummyMemoryAddress = unsigned int;

/**
 * \brief A substitute for a not-initialized address.
 */
static constexpr DummyMemoryAddress NULL_ADDRESS = 0;


/**
 * \brief Represents an abstract assembler operation in the parser-internal
 * intermediate form.
 */
class IntermediateOperation {
 public:
  /**
   * \brief Instantiates a new IntermediateOperation with the given arguments.
   * (only for subclass use!)
   * \param lines The line interval the operation occupies.
   * \param labels The vector of labels assigned to the operation.
   * \param name The name of the operation.
   */
  IntermediateOperation(const LineInterval& lines,
                        const std::vector<std::string>& labels,
                        const std::string& name)
  : _lines(lines), _labels(labels), _name(name), _address(NULL_ADDRESS) {
  }

  /**
   * \brief Executes the given operation (somehow).
   * \param finalRepresentator The FinalRepresentation for possible output.
   * \param table The SymbolTable for possible replacements.
   * \param generator The generator to transform the instructions.
   * \param state The CompileState to log possible errors.
   */
  virtual void execute(FinalRepresentation& finalRepresentator,
                       const SymbolTable& table,
                       const SyntaxTreeGenerator& generator,
                       CompileState& state) = 0;

  /**
   * \brief Enhances the symbol table by the labels of the operation.
   * \param table The SymbolTable to insert into.
   * \param state The CompileState to log possible errors.
   */
  virtual void enhanceSymbolTable(SymbolTable& table, CompileState& state);

  /**
   * \brief Returns the memory address.
   * \return The memory address.
   */
  DummyMemoryAddress address() {
    return _address;
  }

 protected:
  /**
   * \brief Reserves space in memory for the operation and sets the address.
   */
  virtual void determineMemoryPosition() = 0;

  /**
   * \brief The internal line interval.
   */
  LineInterval _lines;

  /**
   * \brief The internal label list.
   */
  std::vector<std::string> _labels;

  /**
   * \brief The internal operation name.
   */
  std::string _name;

  /**
   * \brief The internal memory address.
   */
  DummyMemoryAddress _address;
};

#endif
