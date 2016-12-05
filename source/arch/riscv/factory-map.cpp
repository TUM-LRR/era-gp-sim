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

#include <string>

#include "arch/riscv/factory-map.hpp"

namespace riscv {

FactoryMap::AbstractFacade::AbstractFacade(FactoryMap& factories)
: _factories(factories) {
}

FactoryMap::AbstractFacade::~AbstractFacade() = default;

const FactoryMap::Factory&
FactoryMap::get(const std::string& instructionName) const {
  auto iterator = _map.find(instructionName);
  assert(iterator != _map.end());

  return iterator->second;
}

const FactoryMap::Factory& FactoryMap::
operator[](const std::string& instructionName) const {
  return this->get(instructionName);
}

FactoryMap::Node
FactoryMap::create(const std::string& instructionName,
                   const InstructionInformation& information) const {
  return get(instructionName)(information);
}

FactoryMap::Node FactoryMap::
operator()(const std::string& instructionName,
           const InstructionInformation& information) const {
  return create(instructionName, information);
}
}
