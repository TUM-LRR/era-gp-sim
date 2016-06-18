#include <assert.h>

#include "arch/common/instruction-set.hpp"

InstructionSet::InstructionSet() = default;

InstructionSet::InstructionSet(List instructions) : super(instructions) {
}

InstructionSet& InstructionSet::addInstructions(List instructions) {
	assert(instructions.size() > 0);
	return addInstructions<List>(instructions);
}

InstructionSet&
InstructionSet::addInstruction(const InstructionInformation& instruction) {
	_container.emplace_back(instruction);

	return *this;
}

bool InstructionSet::isValid() const noexcept {
	// clang-format off
	return Utility::allOf(_container, [](auto& instruction) {
		return instruction.isValid();
	});
	// clang-format on
}
