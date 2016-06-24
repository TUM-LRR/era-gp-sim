#include "include/parser/parser.hpp"

std::string Parser::getSyntaxRegister(std::string &name) {
	return "\\b"+name+"\\b";
}

std::string Parser::getSyntaxInstruction(std::string &name) {
	return "\\b"+name+"\\b";
}

std::string Parser::getSyntaxImmediate() {
	return R"(\b\d+\b)";
}