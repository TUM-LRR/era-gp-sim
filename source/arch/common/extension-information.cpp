#include <assert.h>

#include "arch/common/extension-information.hpp"
#include "arch/common/instruction-set.hpp"
#include "common/utility.hpp"

ExtensionInformation::ExtensionInformation() noexcept = default;

ExtensionInformation::ExtensionInformation(const std::string& name)
: _instructions(std::make_unique<InstructionSet>()) {
	// For constraint checking
	this->name(name);
}

ExtensionInformation::ExtensionInformation(const ExtensionInformation& other)
: _name(other._name)
, _endianness(other._endianness)
, _alignmentBehavior(other._alignmentBehavior)
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
	swap(_alignmentBehavior, other._alignmentBehavior);
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
	_endianness = endianness;
	return *this;
}

Optional<ExtensionInformation::Endianness>
ExtensionInformation::getEndianness() const noexcept {
	return _endianness;
}

ExtensionInformation&
ExtensionInformation::alignmentBehavior(AlignmentBehavior alignmentBehavior) {
	_alignmentBehavior = alignmentBehavior;
	return *this;
}

Optional<ExtensionInformation::AlignmentBehavior>
ExtensionInformation::getAlignmentBehavior() const noexcept {
	return _alignmentBehavior;
}

ExtensionInformation& ExtensionInformation::wordSize(size_t wordSize) {
	_wordSize = wordSize;
	return *this;
}

// clang-format off
Optional<ExtensionInformation::size_t>
ExtensionInformation::getWordSize() const noexcept {
	return _wordSize;
}
// clang-format on

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

	if (otherExtension._alignmentBehavior) {
		_alignmentBehavior = otherExtension._alignmentBehavior;
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
	if (!_alignmentBehavior) return false;
	if (!_wordSize) return false;

	// Check the basic stuff
	return isValid();
}
