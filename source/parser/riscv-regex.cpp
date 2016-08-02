#include "parser/riscv-regex.hpp"
#include "parser/riscv-parser.hpp"

#include <cassert>

RiscvParser::RiscvRegex::RiscvRegex()
: line_regex_{
      "\\s*"
      "(?:(\\w+)\\s*\\:)?"// Label group
      "\\s*"
      "(([[:alpha:]]+)"// Instruction group
      "\\s*"
      "(\\w+)"// Parameter group
      "\\s*,\\s*"
      "(\\w+)"// Parameter group
      "(?:\\s*,\\s*"
      "(\\w+))?)?"// Parameter group
      "\\s*"
      "(?:;.*)?"// Comment group
  } {
}

void RiscvParser::RiscvRegex::matchLine(const std::string &line) {
  std::regex_match(line, matches_, line_regex_);
}

bool RiscvParser::RiscvRegex::isValid() {
  return !matches_.empty();
}

bool RiscvParser::RiscvRegex::hasLabel() {
  return matches_[1].matched;
}

bool RiscvParser::RiscvRegex::hasInstruction() {
  return matches_[2].matched;
}

std::string RiscvParser::RiscvRegex::getLabel() {
  return matches_[1].str();
}

std::string RiscvParser::RiscvRegex::getInstruction() {
  return matches_[3].str();
}

std::string RiscvParser::RiscvRegex::getParameter(int n) {
  assert(n >= 0 && n < getParameterCount());
  assert(4 + n < matches_.size());
  return matches_[4 + n];
}

int RiscvParser::RiscvRegex::getParameterCount() {
  return matches_[6].matched ? 3 : 2;
}