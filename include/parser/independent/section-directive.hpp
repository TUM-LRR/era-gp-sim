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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_SECTION_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_SECTION_DIRECTIVE_HPP

#include <string>
#include <vector>

#include "parser/common/final-command.hpp"
#include "parser/independent/intermediate-directive.hpp"

class MemoryAllocator;
class SymbolTable;
class SyntaxTreeGenerator;
class CompileErrorList;
class MemoryAccess;

/**
 * Represents a section directive in code.
 */
class SectionDirective : public IntermediateDirective {
 public:
  /**
   * Creates a new section directive with the given section.
   *
   * \param positionInterval The interval of the command.
   * \param labels The associated labels with the command (doesn't make sense
   * for this one!?).
   * \param name The name of the command, might not be equal to the section. It
   * has no meaning for the directive.
   * \param arguments Arguments of the directive. First should be section name.
   */
  SectionDirective(const CodePositionInterval& positionInterval,
                   const PositionedStringVector& labels,
                   const PositionedString& name,
                   const PositionedStringVector& arguments);

  /**
     * Sets the section to the specified value.
     *
     * \param immutable Some constant arguments which might be helpful.
     * \param errors The compile error list to note down any errors.
     * \param allocator The allocator to reserve memory.
     * \param tracker The section tracker so we know in which section to reserve
   * our data.
     */
  virtual void allocateMemory(const PreprocessingImmutableArguments& immutable,
                              CompileErrorList& errors,
                              MemoryAllocator& allocator,
                              SectionTracker& tracker);

  /**
* Finalizes a section directive.
*/
  virtual ~SectionDirective() = default;

  /**
   * \return The section name.
   */
  const PositionedString& section() const noexcept;

  /**
   * \return True, if the section name is specified, else false.
   */
  bool hasName() const noexcept;

 private:
  /**
   * True, if the section name is specified, else false.
   */
  bool _hasName;

  /**
   * The section to change to.
   */
  PositionedString _section;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_SECTION_DIRECTIVE_HPP */
