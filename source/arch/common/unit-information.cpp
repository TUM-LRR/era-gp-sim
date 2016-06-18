#include <assert.h>

#include "arch/common/unit-information.hpp"

UnitInformation::UnitInformation(const std::string& name) {
	// To check constraints
	this->name(name);
}

UnitInformation& UnitInformation::name(const std::string& name) noexcept {
	assert(!name.empty());
	_name = name;

	return *this;
}

const std::string& UnitInformation::getName() const noexcept {
	return _name;
}

UnitInformation& UnitInformation::addRegisters(List regs) {
	assert(regs.size() > 0);
	return addRegisters<List>(regs);
}

UnitInformation& UnitInformation::addRegister(const RegisterInformation& reg) {
	_container.emplace_back(reg);

	return *this;
}

bool UnitInformation::isValid() const noexcept {
	if (_name.empty()) return false;
	// clang-format off
	return Utility::allOf(_container, [](auto& reg) {
		return reg.isValid();
	});
	// clang-format on
}
