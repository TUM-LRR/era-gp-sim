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

#ifndef ERAGPSIM_PARSER_CONSTANT_DIRECTIVE_HPP_
#define ERAGPSIM_PARSER_CONSTANT_DIRECTIVE_HPP_

#include "parser/intermediate-directive.hpp"

/**
 * Represents a constant directive in code.
 */
class ConstantDirective : public IntermediateDirective {
 public:
  ConstantDirective(const LineInterval& lines,
                    const std::vector<std::string>& labels,
                    const std::string& name,
                    const std::vector<std::string>& arguments);

  virtual void execute(const ExecuteImmutableArguments& immutable,
                       const CompileErrorAnnotator& annotator,
                       FinalRepresentation& finalRepresentator,
                       MemoryAccess& memoryAccess) override;

  virtual void
  enhanceSymbolTable(const EnhanceSymbolTableImmutableArguments& immutable,
                     const CompileErrorAnnotator& annotator,
                     SymbolGraph& graph) override;

 private:
  std::vector<std::string> _arguments;
  std::string _expression;
};

#endif /* ERAGPSIM_PARSER_CONSTANT_DIRECTIVE_HPP_ */
