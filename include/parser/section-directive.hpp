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

#ifndef ERAGPSIM_PARSER_SECTION_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_SECTION_DIRECTIVE_HPP

#include <string>
#include <vector>

#include "parser/intermediate-directive.hpp"

class MemoryAllocator;
#include "parser/final-command.hpp"
class SymbolTable;
class SyntaxTreeGenerator;
class CompileErrorList;
class MemoryAccess;

/**
 * \brief Represents a section directive in code.
 */
class SectionDirective : public IntermediateDirective {
 public:
  /**
   * \brief Creates a new section directive with the given section.
   * \param positionInterval The interval of the command.
   * \param labels The associated labels with the command (doesn't make sense
   * with this one!?).
   * \param name The name of the command, might not be equal to the section. It
   * has no meaning for the directive.
   * \param arguments Arguments of the directive. First should be section name.
   */
  SectionDirective(const CodePositionInterval& positionInterval,
                   const std::vector<PositionedString>& labels,
                   const PositionedString& name,
                   const std::vector<PositionedString>& arguments);

  /**
   * \brief Executes the section directive and sets the compiler section to the
   * corresponding value.
   * \param commandOutput The representation to output commands to
   * (unused).
   * \param table The symbol table for symbol storage (unused).
   * \param generator The syntax tree generator, unused here.
   * \param state The compile state which denotes the current section of the
   * compiler.
   */
  virtual void execute(const ExecuteImmutableArguments& immutable,
                       CompileErrorList& errors,
                       FinalCommandVector& commandOutput,
                       MemoryAccess& memoryAccess);

  virtual void allocateMemory(const PreprocessingImmutableArguments& immutable,
                              CompileErrorList& errors,
                              MemoryAllocator& allocator,
                              SectionTracker& tracker);

 private:
  bool _hasName;

  /**
   * \brief The section to change to.
   */
  PositionedString _section;
};

#endif /* ERAGPSIM_PARSER_SECTION_DIRECTIVE_HPP_ */
