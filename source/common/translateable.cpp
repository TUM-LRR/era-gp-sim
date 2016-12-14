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

Translateable::Translateable(const std::string &baseString,
    const StringList &operands)
    : _baseString(baseString), _operands(operands.size()) {
  auto i = 0;
  for (const auto &stringOp : operands) {
    _operands[i] = std::make_shared<Translateable>(stringOp);
    ++i;
  }
}

Translateable::Translateable(const std::string &baseString,
    const TranslateableRefList &operands)
    : _baseString(baseString), _operands(operands.size()) {
  auto i = 0;
  for (Translateable &refOp : operands) {
    _operands[i] = std::shared_ptr<Translateable>(&refOp);
    ++i;
  }
}

QString Translateable::translate(const QApplication &translater) const {
  // translate the base string (still filled with the %s)
  QString translatedTotal = translater.tr(_baseString.c_str());
  for (const auto &op : _operands) {
    // translate recursively all operands and insert them into the base string
    translatedTotal = translatedTotal.arg(op->translate(translater));
  }
  return translatedTotal;
}

std::string &Translateable::getModifiableBaseString() { return _baseString; }

void Translateable::addOperand(const TranslateablePtr &op) {
  _operands.push_back(op);
}

void Translateable::addOperand(const std::string &op) {
  _operands.push_back(std::make_shared<Translateable>(op));
}
