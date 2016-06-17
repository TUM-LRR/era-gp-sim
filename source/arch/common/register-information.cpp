#include <assert.h>

#include "arch/common/register-information.hpp"

typename RegisterInformation::id_t RegisterInformation::_rollingID = 0;

// clang-format off
RegisterInformation::RegisterInformation(const std::string& name)
: _id(_rollingID++)
, _type(Type::INTEGER)
, _size(32) {
	this->name(name);
}
// clang-format on

RegisterInformation& RegisterInformation::name(const std::string& name) {
	assert(!_name.empty());
	_name = name;

	return *this;
}

const std::string& RegisterInformation::name() const {
	return _name;
}

RegisterInformation& RegisterInformation::size(size_t bit_size) {
	assert(bit_size > 0);
	_size = bit_size;

	return *this;
}

RegisterInformation::size_t RegisterInformation::size() const {
	return _size;
}

RegisterInformation& RegisterInformation::id(id_t id) {
	_id = id;

	return *this;
}

RegisterInformation::id_t RegisterInformation::id() const {
	return _id;
}

RegisterInformation& RegisterInformation::type(Type type) {
	_type = type;

	return *this;
}

RegisterInformation::Type RegisterInformation::type() const {
	return _type;
}

bool RegisterInformation::isHardwired() const {
	return static_cast<bool>(_constant);
}

RegisterInformation& RegisterInformation::alias(const std::string& alias) {
	_aliases.emplace_back(alias);

	return *this;
}

RegisterInformation&
RegisterInformation::aliases(std::initializer_list<std::string> aliases) {
	_aliases.insert(_aliases.end(), aliases);

	return *this;
}

const std::vector<std::string>& RegisterInformation::aliases() const {
	return _aliases;
}

bool RegisterInformation::hasAliases() const {
	return !_aliases.empty();
}

RegisterInformation& RegisterInformation::enclosing(id_t id) {
	assert(id != _id);
	_enclosing = id;

	return *this;
}

RegisterInformation::Optional<RegisterInformation::id_t>
RegisterInformation::enclosing() const {
	return _enclosing;
}

RegisterInformation&
RegisterInformation::constituents(std::initializer_list<id_t> constituents) {
	_constituents.insert(_constituents.end(), constituents);

	return *this;
}

RegisterInformation& RegisterInformation::constituent(id_t id) {
	_constituents.emplace_back(id);

	return *this;
}

const std::vector<RegisterInformation::id_t>&
RegisterInformation::constituents() const {
	return _constituents;
}

bool RegisterInformation::hasConstituents() const {
	return !_constituents.empty();
}
