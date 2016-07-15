#include "parser/riscv-parser.hpp"

#include <cassert>
#include <iostream>
#include <regex>
#include <sstream>

std::vector<std::unique_ptr<AbstractSyntaxTreeNode>>
RiscvParser::parse(const std::string &text, ParserMode parserMode) {
  std::vector<std::unique_ptr<AbstractSyntaxTreeNode>> syntax_nodes;
  std::istringstream stream{text};
  compile_state_.errorList.clear();

  std::regex line_regex{
      "\\s*"
      "(?:(\\w+)\\s*\\:)?"// Label group
      "\\s*"
      "([[:alpha:]]+)"// Instruction group
      "\\s*"
      "(\\w+)"// Parameter group
      "\\s*,\\s*"
      "(\\w+)"// Parameter group
      "\\s*,\\s*"
      "(\\w+)"// Parameter group
      "\\s*"};
  std::smatch matches;

  for (std::string line; std::getline(stream, line);) {
    std::regex_match(line, matches, line_regex);

    assert(matches.empty() ^ matches.size() == 6);

    std::cout << line << "\n\t" << matches.empty() << "\n\t" << matches.size()
              << "\n\tLabel: " << matches[1].str()
              << "\n\tOP: " << matches[2].str()
              << "\n\tPARM1: " << matches[3].str()
              << "\n\tPARM2: " << matches[4].str()
              << "\n\tPARM3: " << matches[5].str() << std::endl;
  }

  return syntax_nodes;
}

std::vector<CompileError> RiscvParser::getErrorList() {
  return compile_state_.errorList;
}