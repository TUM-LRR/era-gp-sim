/* C++ Assembler Interpreter
 * Copyright (C) 2016 Chair of Computer Architecture
 * at Technical University of Munich
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.*/

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
