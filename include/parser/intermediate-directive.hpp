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

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_DIRECTIVE_HPP_
#define ERAGPSIM_PARSER_INTERMEDIATE_DIRECTIVE_HPP_

#include "intermediate-operation.hpp"

// This is a dummy class for a future implementation.
/**
 * \class IntermediateDirective
 * \brief Represents a directive in the parser-internal intermediate form.
 */
class IntermediateDirective : public IntermediateOperation {
 public:
  /**
   * \fn IntermediateDirective::IntermediateDirective(const LineInterval& lines,
   * const std::vector<std::string>& labels, const std::string& name)
   * \brief Instantiates a new IntermediateDirective with the given arguments.
   * (only for subclass use!)
   * \param lines The line interval the operation occupies.
   * \param labels The vector of labels assigned to the operation.
   * \param name The name of the operation.
   */
  IntermediateDirective(const LineInterval& lines,
                        const std::vector<std::string>& labels,
                        const std::string& name)
  : IntermediateOperation(lines, labels, name) {
  }

  /**
   * \fn IntermediateDirective::IntermediateDirective(const
   * IntermediateDirective& other)
   * \brief Default copy constructor.
   * \param other The source IntermediateDirective instance.
   */
  IntermediateDirective(const IntermediateDirective& other) = default;

  /**
   * \fn IntermediateDirective::IntermediateDirective(IntermediateDirective&&
   * other)
   * \brief Default move constructor.
   * \param other The source IntermediateDirective instance.
   */
  IntermediateDirective(IntermediateDirective&& other) = default;

  /**
   * \fn IntermediateDirective::operator =(const IntermediateDirective& other)
   * \brief Default copy assignment operator.
   * \param other The source IntermediateDirective instance.
   */
  IntermediateDirective&
  operator=(const IntermediateDirective& other) = default;

  /**
   * \fn IntermediateDirective::operator =(IntermediateDirective&& other)
   * \brief Default move assignment operator.
   * \param other The source IntermediateDirective instance.
   */
  IntermediateDirective& operator=(IntermediateDirective&& other) = default;

  /**
   * \fn IntermediateDirective::~IntermediateDirective()
   * \brief Default destructor.
   */
  ~IntermediateDirective() = default;
};

#endif
