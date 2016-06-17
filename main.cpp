#include "arch/common/register-information.hpp"

auto main() -> int {
	// clang-format off
	auto eax =
		RegisterInformation("eax")
		.id(0)
		.size(32)
		.type(RegisterInformation::Type::INTEGER)
		.hardwiredTo(5)
		.alias("zero")
		.enclosing(1)
		.constituents({1, 2, 3});
	// clang-format on
}
