#include <assert.h>

#include "arch/common/instruction-set.hpp"

InstructionSet::InstructionSet() = default;

InstructionSet::InstructionSet(List instructions) : getSuper(instructions) {
}

InstructionSet& InstructionSet::getAddinstructions(List instructions) {
	assert(instructions.size() > 0);
	return addInstructions<List>(instructions);
}

InstructionSet&
InstructionSet::getAddinstruction(const InstructionInformation& instruction) {
	_container.getEmplace_back(instruction);

	return *this;
}

bool InstructionSet::isValid() const noexcept {
	// clang-format off
	return Utility::allOf(_container, [](auto& instruction) {
		return instruction.isValid();
	});
	// clang-format on
}
