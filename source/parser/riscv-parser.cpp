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

#include "parser/riscv-parser.hpp"

#include <cassert>
#include <iostream>
#include <regex>
#include <sstream>
#include "arch/common/node-factory-collection-maker.hpp"
#include "parser/intermediate-instruction.hpp"
#include "parser/intermediate-representator.hpp"
#include "parser/riscv-regex.hpp"

RiscvParser::RiscvParser(const Architecture &architecture) {
  _factory_collection = NodeFactoryCollectionMaker::CreateFor(architecture);
}

FinalRepresentation
RiscvParser::parse(const std::string &text, ParserMode parserMode) {
  IntermediateRepresentator intermediate;
  std::istringstream stream{text};

  // Initialize compile state
  _compile_state.errorList.clear();
  _compile_state.position = CodePosition(0, 0);
  _compile_state.mode     = parserMode;


  RiscvRegex line_regex;
  std::vector<std::string> labels, sources, targets;

  for (std::string line; std::getline(stream, line);) {
    _compile_state.position.first++;
    line_regex.matchLine(line);
    if (!line_regex.isValid()) {
      // Add syntax error if line regex doesnt match
      _compile_state.errorList.push_back(
          CompileError{"Syntax Error",
                       _compile_state.position,
                       CompileErrorSeverity::ERROR});
    } else {
      // Collect labels until next instruction
      if (line_regex.hasLabel()) {
        labels.push_back(line_regex.getLabel());
      }

      if (line_regex.hasInstruction()) {
        // Collect source and target parameters
        for (int i = 0; i < line_regex.getParameterCount(); i++) {
          if (i == 0)
            targets.push_back(line_regex.getParameter(i));
          else
            sources.push_back(line_regex.getParameter(i));
        }

        intermediate.insertCommand(
            IntermediateInstruction{LineInterval{_compile_state.position.first,
                                                 _compile_state.position.first},
                                    labels,
                                    line_regex.getInstruction(),
                                    sources,
                                    targets});

        labels.clear();
        targets.clear();
        sources.clear();
      }
    }
  }

  return intermediate.transform(SyntaxTreeGenerator{_factory_collection},
                                _compile_state);
}
