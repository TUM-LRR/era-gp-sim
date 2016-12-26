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

#include "common/translateable.hpp"

Translateable::Translateable() {
}

Translateable::Translateable(const std::string &baseString)
: _baseString(baseString) {
}

Translateable::Translateable(const char *baseString)
: Translateable(std::string{baseString}) {
}

Translateable::Translateable(const std::string &baseString,
                             const NO_TR_POSSIBLE &key)
: Translateable(baseString) {
}

Translateable::Translateable(const char *base,
                             const std::initializer_list<std::string> arguments)
: _baseString(base) {
  for (const auto &argument : arguments) {
    addOperand(argument);
  }
}

std::string &Translateable::getModifiableBaseString() {
  return _baseString;
}

void Translateable::addOperand(const TranslateablePtr &op) {
  _operands.push_back(op);
}

void Translateable::addOperand(const std::string &op) {
  _operands.push_back(createShared(op));
}

Translateable::TranslateablePtr
Translateable::createShared(const Translateable::TranslateablePtr &arg) {
  return arg;
}

const std::string &Translateable::getBaseString() const {
  return _baseString;
}

const std::vector<Translateable::TranslateablePtr> &
Translateable::getOperands() const {
  return _operands;
}
