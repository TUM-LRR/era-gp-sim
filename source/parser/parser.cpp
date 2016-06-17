#include "../../include/parser/parser.hpp"

std::string Parser::getSyntaxRegister(std::string &name) {
	return name;
}

std::string Parser::getSyntaxInstruction(std::string &name) {
	return name;
}

std::string Parser::getSyntaxImmediate() {
	return R"(\b\d+\b)";
}