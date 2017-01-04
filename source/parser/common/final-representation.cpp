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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "parser/common/final-representation.hpp"

#include <cstddef>

#include "arch/common/abstract-instruction-node.hpp"

FinalRepresentation::AddressMapping FinalRepresentation::createMapping() const {
  AddressMapping mapping;

  for (auto i : Utility::range<size_t>(0, _commandList.size())) {
    mapping[_commandList[i].address()] = i;
  }

  return mapping;
}

FinalRepresentation::FinalRepresentation(
    const FinalCommandVector& commandList,
    const CompileErrorList& errorList,
    const MacroInformationVector& macroList)
: _commandList(commandList), _errorList(errorList), _macroList(macroList) {
}
const FinalCommandVector& FinalRepresentation::commandList() const noexcept {
  return _commandList;
}
const CompileErrorList& FinalRepresentation::errorList() const noexcept {
  return _errorList;
}
const MacroInformationVector& FinalRepresentation::macroList() const noexcept {
  return _macroList;
}
