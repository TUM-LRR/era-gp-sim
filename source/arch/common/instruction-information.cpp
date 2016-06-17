#include <assert.h>

#include "arch/common/instruction-information.hpp"

InstructionInformation::InstructionInformation(const std::string& name,
																							 opcode_t opcode)
: _name(name), _opcode(opcode) {
}

InstructionInformation& InstructionInformation::name(const std::string& name) {
	assert(!name.empty());
	_name = name;

	return *this;
}

const std::string& InstructionInformation::getName() const noexcept {
	return _name;
}

InstructionInformation& InstructionInformation::opcode(opcode_t opcode) {
	_opcode = opcode;
	return *this;
}

InstructionInformation::opcode_t InstructionInformation::getOpcode() const
		noexcept {
	return _opcode;
}

bool InstructionInformation::isValid() const noexcept {
	return !_name.empty();
}
