#include <iostream>

#include "arch/common/architecture.hpp"
#include "arch/common/extension-information.hpp"
#include "arch/common/information.hpp"
#include "arch/common/instruction-information.hpp"
#include "arch/common/instruction-set.hpp"
#include "arch/common/register-information.hpp"
#include "arch/common/unit-information.hpp"


auto main() -> int {
	// clang-format off
	auto eax =
		RegisterInformation("eax")
		.id(0)
		.size(32)
		.type(RegisterInformation::Type::INTEGER)
		.hardwiredTo(5)
		.addAlias("zero")
		.enclosing(1)
		.addConstituents({1, 2, 3});
	auto unit = UnitInformation("cpu").addRegister(eax);
	auto add = InstructionInformation("add").key(5);
	auto set = InstructionSet({add});
	auto base = ExtensionInformation("rvi32")
		.alignmentBehavior(Information::AlignmentBehavior::STRICT)
		.endianness(Information::Endianness::LITTLE)
		.wordSize(32)
		.addUnit(unit)
		.addInstructions(set);
	auto arch = Architecture("risc-v", base).validate();
	// clang-format on

	std::cout << arch.getWordSize() << std::endl;
}
