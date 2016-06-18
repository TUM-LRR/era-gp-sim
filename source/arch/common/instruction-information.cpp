#include <assert.h>

#include "arch/common/instruction-information.hpp"

InstructionInformation::InstructionInformation(const std::string& name)
: _name(name) {
}

InstructionInformation::InstructionInformation(const std::string& name,
																							 const InstructionKey& key)
: _name(name), _key(key) {
}

InstructionInformation& InstructionInformation::name(const std::string& name) {
	assert(!name.empty());
	_name = name;

	return *this;
}

const std::string& InstructionInformation::getName() const noexcept {
	return _name;
}

InstructionInformation& InstructionInformation::key(const InstructionKey& key) {
	_key = key;
	return *this;
}

const InstructionInformation::InstructionKey&
InstructionInformation::getKey() const noexcept {
	assert(static_cast<bool>(_key));
	return *_key;
}

bool InstructionInformation::isValid() const noexcept {
	return !_name.empty() && static_cast<bool>(_key);
}
