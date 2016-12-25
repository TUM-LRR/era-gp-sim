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

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_DIRECTIVE_FACTORY_HPP_
#define ERAGPSIM_PARSER_INTERMEDIATE_DIRECTIVE_FACTORY_HPP_

#include <memory>
#include <unordered_map>
#include "parser/intermediate-directive.hpp"

class IntermediateRepresentator;
class CompileErrorList;

/**
 * Provides methods for the instantiation of different RiscvParser directives.
 */
namespace RiscVDirectiveFactory {

/**
 * Instantiates a Directive by name and inserts it into an
 * IntermediateRepresentator if valid.
 *
 * \param positionInterval        The line interval the directive occupies.
 * \param labels       The labels assigned to this directive.
 * \param name         The name of the directive.
 * \param arguments    The arguments of the directive.
 * \param intermediate The intermediate represenator to insert the new directive
 *                     into.
 * \param state        The current compile state to record errors.
 */
void create(const CodePositionInterval &positionInterval,
            const std::vector<PositionedString> &labels,
            const PositionedString &name,
            const std::vector<PositionedString> &arguments,
            IntermediateRepresentator &intermediate,
            CompileErrorList &errors);

/**
 * Maps names to directives.
 */
extern const std::unordered_map<
    std::string,
    std::function<std::shared_ptr<IntermediateDirective>(
        const CodePositionInterval &,
        const std::vector<PositionedString> &,
        const PositionedString &,
        const std::vector<PositionedString> &)>>
    mapping;
}

#endif
