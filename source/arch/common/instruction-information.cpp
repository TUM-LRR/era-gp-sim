/*
* C++ Assembler Interpreter
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <cassert>
#include <string>

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
