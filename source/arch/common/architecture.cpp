#include <assert.h>

#include "arch/common/architecture.hpp"
#include "arch/common/extension-information.hpp"

Architecture::Architecture(const std::string& name,
													 const ExtensionInformation& base)
: _validated(false) {
	assert(base.isValidBase());
	// Only copy after
	_base = std::make_unique<ExtensionInformation>(base))
	// For constraints
	this->name(name);
}

Architecture::Architecture() = default;

Architecture::Architecture(const Architecture& other)
: _name(other._name)
, _base(std::make_unique<ExtensionInformation>(*other._base))
, _validated(other._validated) {
}

Architecture::Architecture(Architecture&& other) noexcept : Architecture() {
	swap(other);
}

Architecture& Architecture::operator=(Architecture& other) {
	swap(other);
	return *this;
}

Architecture::~Architecture() = default;

void Architecture::swap(Architecture& other) noexcept {
	using std::swap;

	swap(_name, other._name);
	swap(_base, other._base);
	swap(_validated, other._validated);
}

void swap(Architecture& first, Architecture& second) noexcept {
	first.swap(second);
}

Architecture& Architecture::operator+=(const ExtensionInformation& extension) {
	return extend(extension);
}

Architecture Architecture::
operator+(const ExtensionInformation& extension) const {
	auto temp = *this;
	temp += extension;

	return temp;
}

Architecture& Architecture::name(const std::string& name) {
	assert(!name.empty());
	_name = name;

	return *this;
}

const std::string& Architecture::getName() const noexcept {
	return _name;
}

Architecture::Endianness Architecture::getEndianness() const noexcept {
	assert(isValidated());
	return _base->getEndianness();
}

Architecture::Alignment Architecture::getAlignment() const noexcept {
	assert(isValidated());
	return _base->getAlignment().value();
}

/**
 * Returns the word size of the extension (in bits), if any.
 */
Architecture::word_size_t Architecture::getWordSize() const noexcept {
	assert(isValidated());
	return _base->getWordSize().value();
}

const Architecture::UnitContainer& Architecture::getUnits() const {
	assert(isValidated());
	return _base->getUnits();
}

const InstructionSet& Architecture::getInstructions() const {
	assert(isValidated());
	return _base->getInstructions().value();
}

Architecture& Architecture::extend(const ExtensionInformation& extension) {
	assert(_base != nullptr);

	_base->merge(extension);
	_validated = false;

	return *this;
}

Architecture& Architecture::validate() {
	assert(_isValid());
	_validated = true;

	return *this;
}

bool Architecture::isValidated() const noexcept {
	return _validated;
}

bool Architecture::isValid() const noexcept {
	if (_name.empty()) return false;
	if (_base == nullptr) return false;
	if (!_base->isValidBase()) return false;

	return true;
}
