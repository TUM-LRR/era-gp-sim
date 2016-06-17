#include <assert.h>

#include "arch/common/extension-information.hpp"
#include "arch/common/instruction-set.hpp"
#include "common/utility.hpp"

ExtensionInformation::ExtensionInformation(const std::string& name)
: _instructions(std::make_unique<InstructionSet>()) {
	// For constraint checking
	this->name(name);
}

ExtensionInformation::ExtensionInformation(const ExtensionInformation& other)
: _name(other._name)
, _endianness(other._endianness)
, _alignment(other._alignment)
, _wordSize(other._wordSize)
, _instructions(std::make_unique<InstructionSet>(*other._instructions))
, _units(other._units) {
}

ExtensionInformation::ExtensionInformation(
		ExtensionInformation&& other) noexcept
: ExtensionInformation() {
	swap(other);
}

ExtensionInformation& ExtensionInformation::
operator=(ExtensionInformation other) {
	swap(other);

	return *this;
}

ExtensionInformation::~ExtensionInformation() = default;

void ExtensionInformation::swap(ExtensionInformation& other) noexcept {
	using std::swap;

	swap(_name, other._name);
	swap(_endianness, other._endianness);
	swap(_alignment, other._alignment);
	swap(_wordSize, other._wordSize);
	swap(_instructions, other._instructions);
	swap(_units, other._units);
}

void swap(ExtensionInformation& first, ExtensionInformation& second) noexcept {
	first.swap(second);
}

ExtensionInformation& ExtensionInformation::
operator+=(const ExtensionInformation& other) {
	return merge(other);
}

ExtensionInformation ExtensionInformation::
operator+(const ExtensionInformation& other) const {
	auto temp = *this;
	temp += other;

	return temp;
}

ExtensionInformation& ExtensionInformation::name(const std::string& name) {
	assert(!name.empty());
	_name = name;

	return *this;
}

const std::string& ExtensionInformation::getName() const noexcept {
	return _name;
}

ExtensionInformation& ExtensionInformation::endianness(Endianness endianness) {
	*_endianness = endianness;

	return *this;
}

Optional<ExtensionInformation::Endianness>
ExtensionInformation::getEndianness() const noexcept {
	return _endianness;
}

ExtensionInformation& ExtensionInformation::alignment(Alignment alignment) {
	*_alignment = alignment;

	return *this;
}

Optional<ExtensionInformation::Alignment>
ExtensionInformation::getAlignment() const noexcept {
	return _alignment;
}

ExtensionInformation& ExtensionInformation::wordSize(size_t wordSize) {
	_wordSize = wordSize;

	return *this;
}

Optional<ExtensionInformation::size_t> ExtensionInformation::getWordSize() const
		noexcept {
	return _wordSize;
}

ExtensionInformation&
ExtensionInformation::addInstructions(const InstructionSet& instructions) {
	assert(_instructions != nullptr);
	*_instructions += instructions;

	return *this;
}

ExtensionInformation&
ExtensionInformation::setInstructions(const InstructionSet& instructions) {
	assert(_instructions != nullptr);
	*_instructions += instructions;

	return *this;
}

const InstructionSet& ExtensionInformation::getInstructions() const noexcept {
	assert(_instructions != nullptr);
	return *_instructions;
}

ExtensionInformation& ExtensionInformation::addUnits(UnitList units) {
	assert(units.size() > 0);
	return addUnits<UnitList>(units);
}

ExtensionInformation&
ExtensionInformation::addUnit(const UnitInformation& unit) {
	_units.emplace_back(unit);

	return *this;
}

const ExtensionInformation::UnitContainer&
ExtensionInformation::getUnits() const noexcept {
	return _units;
}

ExtensionInformation& ExtensionInformation::merge(ExtensionList list) {
	assert(list.size() > 0);
	return merge<ExtensionList>(list);
}

ExtensionInformation&
ExtensionInformation::merge(const ExtensionInformation& otherExtension) {
	if (otherExtension._endianness) {
		_endianness = otherExtension._endianness;
	}

	if (otherExtension._alignment) {
		_alignment = otherExtension._alignment;
	}

	if (otherExtension._wordSize) {
		_wordSize = otherExtension._wordSize;
	}

	*_instructions += otherExtension.getInstructions();
	Utility::concatenate(_units, otherExtension.getUnits());

	return *this;
}

bool ExtensionInformation::isValid() const noexcept {
	if (_name.empty()) return false;
	if (!_instructions->isValid()) return false;
	if (!Utility::allOf(_units, [](auto& unit) { return unit.isValid(); })) {
		return false;
	}

	return true;
}

bool ExtensionInformation::isValidBase() const noexcept {
	if (!_endianness) return false;
	if (!_alignment) return false;
	if (!_wordSize) return false;

	// Check the basic stuff
	return isValid();
}
