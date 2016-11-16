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

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_INTERMEDIATE_DIRECTIVE_HPP

#include "parser/intermediate-operation.hpp"

/**
 * \brief Represents a directive in the parser-internal intermediate form.
 */
class IntermediateDirective : public IntermediateOperation {
 public:
  /**
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
};

#endif
