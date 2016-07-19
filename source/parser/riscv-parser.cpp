#include "parser/riscv-parser.hpp"

#include <cassert>
#include <iostream>
#include <regex>
#include <sstream>
#include "parser/intermediate-instruction.hpp"
#include "parser/intermediate-representator.hpp"
#include "parser/riscv-regex.hpp"

std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>
RiscvParser::parse(const std::string &text, ParserMode parserMode) {
  IntermediateRepresentator intermediate;
  std::istringstream stream{text};

  compile_state_.errorList.clear();
  compile_state_.position = CodePosition(0, 0);
  compile_state_.mode     = parserMode;

  RiscvRegex line_regex;
  std::vector<std::string> labels, sources, targets;

  for (std::string line; std::getline(stream, line);) {
    compile_state_.position.first++;
    line_regex.matchLine(line);

    std::cout << compile_state_.position.first << "# " << line;
    if (!line_regex.isValid()) {
      std::cout << "\n\tInvalid\n";
      compile_state_.errorList.push_back(
          CompileError{"Syntax Error",
                       compile_state_.position,
                       CompileErrorSeverity::ERROR});
    } else {
      if (line_regex.hasLabel()) {
        std::cout << "\n\tLabel: " << line_regex.getLabel();
        labels.push_back(line_regex.getLabel());
      }

      if (line_regex.hasInstruction()) {
        std::cout << "\n\tOP: " << line_regex.getInstruction();
        for (int i = 0; i < line_regex.getParameterCount(); i++) {
          if (i == 0)
            sources.push_back(line_regex.getParameter(i));
          else
            targets.push_back(line_regex.getParameter(i));
          std::cout << "\n\tPARM" << (i + 1) << ": "
                    << line_regex.getParameter(i);
        }
        intermediate.insertCommand(
            IntermediateInstruction{LineInterval{compile_state_.position.first,
                                                 compile_state_.position.first},
                                    labels,
                                    line_regex.getInstruction(),
                                    sources,
                                    targets});
      }

      std::cout << std::endl;
    }
  }

  return intermediate.transform(compile_state_);
}

std::vector<CompileError> RiscvParser::getErrorList() {
  return compile_state_.errorList;
}