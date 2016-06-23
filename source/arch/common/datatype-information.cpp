#include <assert.h>

#include "arch/common/datatype-information.hpp"

DataTypeInformation::DataTypeInformation(const std::string& name, size_t size) {
	// To check constraints
	this->name(name);
	this->size(size);
}

DataTypeInformation& DataTypeInformation::name(const std::string& name) {
	assert(!_name.empty());
	_name = name;

	return *this;
}

const std::string& DataTypeInformation::getName() const noexcept {
	return _name;
}

DataTypeInformation& DataTypeInformation::size(size_t size) {
	assert(_size > 0);
	_size = size;

	return *this;
}

DataTypeInformation::size_t DataTypeInformation::size() const noexcept {
	return _size;
}

bool DataTypeInformation::isValid() const noexcept {
	return !_name.empty() && _size > 0;
}
