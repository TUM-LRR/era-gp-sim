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

#ifndef ERAGPSIM_PARSER_MEMORY_RESERVATION_DIRECTIVE_HPP
#define ERAGPSIM_PARSER_MEMORY_RESERVATION_DIRECTIVE_HPP

#include "intermediate-directive.hpp"

class MemoryReservationDirective : public IntermediateDirective {
public:
    /**
   * \brief Instantiates a new IntermediateDirective with the given arguments.
   * (only for subclass use!)
   * \param lines The line interval the operation occupies.
   * \param labels The vector of labels assigned to the operation.
   * \param name The name of the operation.
   */
  MemoryReservationDirective(const LineInterval& lines,
                        const std::vector<std::string>& labels,
                        const std::string& name,
                        int cellSize,
                        const std::vector<std::string>& values)
  : IntermediateDirective(lines, labels, name), _cellSize(cellSize), _values(values) {
  }

  virtual void execute(FinalRepresentation& finalRepresentator,
                       const SymbolTable& table,
                       const SyntaxTreeGenerator& generator,
                       CompileState& state)
{
    
}

protected:
  virtual void determineMemoryPosition()
  {

      //needs to be implemented when memory and so on is finished.
  }  

private:
    int _cellSize;
    std::vector<std::string> _values;
};

#endif /* ERAGPSIM_PARSER_MEMORY_RESERVATION_DIRECTIVE_HPP */
