#include "include/memory-value.hpp"

MemoryValue::MemoryValue(int width) : _data{std::vector<bool>(width, false)} {}

MemoryValue::MemoryValue(const std::vector<bool> data) : _data{data} {}

std::vector<bool>::reference MemoryValue::at(const int index) {}

std::vector<bool>::const_reference MemoryValue::at(const int index) const {}

std::vector<bool>::reference MemoryValue::operator[](const int index) {}

std::vector<bool>::const_reference MemoryValue::
operator[](const int index) const {}

void MemoryValue::flip() {}

bool MemoryValue::operator==(const MemoryValue &second) const {}

std::ostream &operator<<(std::ostream &stream, const MemoryValue &value) {}

int MemoryValue::size() const {}
