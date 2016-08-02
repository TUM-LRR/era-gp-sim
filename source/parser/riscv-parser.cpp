#include "parser/riscv-parser.hpp"

#include <cassert>
#include <iostream>
#include <regex>
#include <sstream>
#include "arch/common/architecture-formula.hpp"
#include "arch/common/node-factory-collection-maker.hpp"
#include "parser/intermediate-instruction.hpp"
#include "parser/intermediate-representator.hpp"
#include "parser/riscv-regex.hpp"
#include "parser/syntax-tree-generator.hpp"

RiscvParser::RiscvParser() {
  ArchitectureFormula arch_form{"riscv", {"rv32i"}};
  factory_collection_ =
      NodeFactoryCollectionMaker::CreateFor(Architecture::Brew(arch_form));
}

FinalRepresentation
RiscvParser::parse(const std::string &text, ParserMode parserMode) {
  IntermediateRepresentator intermediate;
  std::istringstream stream{text};

  // Initialize compile state
  compile_state_.errorList.clear();
  compile_state_.position = CodePosition(0, 0);
  compile_state_.mode     = parserMode;


  RiscvRegex line_regex;
  std::vector<std::string> labels, sources, targets;

  for (std::string line; std::getline(stream, line);) {
    compile_state_.position.first++;
    line_regex.matchLine(line);
    if (!line_regex.isValid()) {
      // Add syntax error if line regex doesnt match
      compile_state_.errorList.push_back(
          CompileError{"Syntax Error",
                       compile_state_.position,
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
            IntermediateInstruction{LineInterval{compile_state_.position.first,
                                                 compile_state_.position.first},
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

  return intermediate.transform(SyntaxTreeGenerator{factory_collection_},
                                compile_state_);
}
