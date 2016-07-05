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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_REPRESENTATOR_HPP_
#define ERAGPSIM_PARSER_INTERMEDIATE_REPRESENTATOR_HPP_

#include <memory>
#include <string>
#include <vector>
#include "../arch/common/abstract-node-factories.hpp"
#include "compile-state.hpp"
#include "final-representation.hpp"
#include "intermediate-operation.hpp"
#include "symbol-table.hpp"

/**
 * \class IntermediateRepresentator
 * \brief Provides methods for collecting and compiling a command list.
 */
class IntermediateRepresentator {
 public:
  /**
   * \fn IntermediateRepresentator::IntermediateRepresentator()
   * \brief Instantiates an IntermediateRepresentator with the default values.
   */
  IntermediateRepresentator() = default;

  /**
   * \fn IntermediateRepresentator::IntermediateRepresentator(const
   * IntermediateRepresentator& other)
   * \brief Default copy constructor.
   * \param other The source IntermediateRepresentator instance.
   */
  IntermediateRepresentator(const IntermediateRepresentator& other) = default;

  /**
   * \fn
   * IntermediateRepresentator::IntermediateRepresentator(IntermediateRepresentator&&
   * other)
   * \brief Default move constructor.
   * \param other The source IntermediateRepresentator instance.
   */
  IntermediateRepresentator(IntermediateRepresentator&& other) = default;

  /**
   * \fn IntermediateRepresentator::operator =(const IntermediateRepresentator&
   * other)
   * \brief Default copy assignment operator.
   * \param other The source IntermediateRepresentator instance.
   */
  IntermediateRepresentator&
  operator=(const IntermediateRepresentator& other) = default;

  /**
   * \fn IntermediateRepresentator::operator =(IntermediateRepresentator&&
   * other)
   * \brief Default move assignment operator.
   * \param other The source IntermediateRepresentator instance.
   */
  IntermediateRepresentator&
  operator=(IntermediateRepresentator&& other) = default;

  /**
   * \fn IntermediateRepresentator::~IntermediateRepresentator()
   * \brief Default destructor.
   */
  ~IntermediateRepresentator() = default;

  /**
   * \fn IntermediateRepresentator::insertCommand(const T& command)
   * \brief Inserts the given command into the command list.
   * \param command The given command.
   * \tparam T The command type.
   */
  template <typename T>
  void insertCommand(const T& command) {
    _commandList.push_back(std::make_unique<T>(command));
  }

  /**
   * \fn IntermediateRepresentator::transform(CompileState& state)
   * \brief Transforms the commands to a syntax tree list.
   * \param state The compile state to report errors.
   * \return The list of syntax trees to be interpreted by the architecture.
   */
  FinalRepresentation transform(CompileState& state);

 private:
  /**
   * \var IntermediateRepresentator::_commandList
   * \brief The internal command list.
   */
  std::vector<std::unique_ptr<IntermediateOperation>> _commandList;
};

#endif
