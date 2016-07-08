#include "parser/parser.hpp"

std::string Parser::getSyntaxRegister(const std::string &name) {
  return "\\b" + name + "\\b";
}

std::string Parser::getSyntaxInstruction(const std::string &name) {
  return "\\b" + name + "\\b";
}

std::string Parser::getSyntaxImmediate() {
  return R"(\b\d+\b)";
}