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

#include "arch/riscv/documentation-builder.hpp"
#include "QtGlobal"
#include "common/assert.hpp"

using Key = DocumentationBuilder::Key;

const std::vector<std::string> DocumentationBuilder::_colors = {
    // blue, red, orange/brown
    "#3366c4", "#66c433", "#c46633"};

DocumentationBuilder::DocumentationBuilder() : _operandCount(0) {}

Translateable DocumentationBuilder::build() {
  assert::that(_hasKey(Key::INSTRUCTION));
  assert::that(_hasKey(Key::SHORT_SYNTAX));
  return Translateable(
      "<b>%1</b>: <code>%2</code><p "
      "style=\"margin-left:5%\">%3</p><br>%4<br>%5<br>%6",
      _components[Key::INSTRUCTION], _components[Key::SHORT_SYNTAX],
      _optional(Key::OPERAND_DESCRIPTION), _optional(Key::SHORT_DESCRIPTION),
      _optional(Key::DETAIL_DESCRIPTION), _optional(Key::OPERAND_RANGE));
}

DocumentationBuilder &DocumentationBuilder::instruction(const std::string &s) {
  _add(Key::INSTRUCTION,
       std::make_shared<Translateable>(s, Translateable::NO_TR_POSSIBLE{}));
  return *this;
}

DocumentationBuilder &DocumentationBuilder::detailDescription(
    const std::string &s) {
  _add(Key::DETAIL_DESCRIPTION,
       std::make_shared<Translateable>(s, Translateable::NO_TR_POSSIBLE{}));
  return *this;
}

DocumentationBuilder &DocumentationBuilder::detailDescription(
    const Translateable::TranslateablePtr &msg) {
  _add(Key::DETAIL_DESCRIPTION, msg);
  return *this;
}

DocumentationBuilder &DocumentationBuilder::operandDescription(
    const std::string &name, const std::string &description) {
  auto it = _components.find(Key::OPERAND_DESCRIPTION);
  Translateable *existingTranslateable = nullptr;
  if (it != _components.end()) {
    existingTranslateable = &(*(it->second));
  } else {
    _components[Key::OPERAND_DESCRIPTION] = std::make_shared<Translateable>("");
    existingTranslateable = &*(_components[Key::OPERAND_DESCRIPTION]);
  }
  auto &baseString = existingTranslateable->getModifiableBaseString();
  baseString += '%';
  baseString += std::to_string(_operandCount + 1);  // will be %1, %2, ...
  existingTranslateable->addOperand(
      "<br><code><span style=\"color:%1\"><b>%2</b></span></code>: %3",
      _nextColor(), name, description);
  ++_operandCount;
  return *this;
}

DocumentationBuilder &DocumentationBuilder::shortDescription(
    const std::string &s) {
  _add(Key::SHORT_DESCRIPTION,
       std::make_shared<Translateable>(
           "<span style=\"background-color: #cccccc\"><b>" + s + "</b></span>",
           Translateable::NO_TR_POSSIBLE{}));
  return *this;
}

DocumentationBuilder &DocumentationBuilder::shortSyntax(StringList operands) {
  auto translateable = std::make_shared<Translateable>("");
  std::string result = "";
  auto i = 0;
  for (const auto &op : operands) {
    result += "<span style=\"color:%";
    result += std::to_string((2 * i) + 1);  // will be %1, %3, ...
    result += "\"><b>%";
    result += std::to_string(2 + 2 * i);  // will be %2, %4, ...
    result += "</b><span>, ";
    translateable->addOperand(_colors[i % _colors.size()]);
    translateable->addOperand(
        std::make_shared<Translateable>(op, Translateable::NO_TR_POSSIBLE{}));
    ++i;
  }

  // remove last ", " and put into translateable
  translateable->getModifiableBaseString() +=
      result.substr(0, result.size() - 2);
  _add(Key::SHORT_SYNTAX, translateable);
  return *this;
}

DocumentationBuilder &DocumentationBuilder::operandRange(
    const std::string &name, int range, bool isSigned) {
  // signed range is from -2^(n-1) to 2^(n-1)-1
  // unsigned range is from 0 to 2^(n)-1
  int lowerBound = isSigned ? -(1 << (range-1)) : 0;
  int upperBound = isSigned ? ((1 << (range-1)) - 1) : ((1 << range) - 1);
  TranslateablePtr ptr = std::make_shared<Translateable>(
      QT_TRANSLATE_NOOP("RISCV instruction help text", "Range of %1: %2 to %3"),
      name, std::to_string(lowerBound), std::to_string(upperBound));
  _add(Key::OPERAND_RANGE, ptr);
  return *this;
}

const std::string &DocumentationBuilder::_nextColor() const {
  return _colors[_operandCount % _colors.size()];
}

Translateable::TranslateablePtr DocumentationBuilder::_optional(
    const Key &key) {
  if (_hasKey(key)) {
    return _components[key];
  } else {
    return std::make_shared<Translateable>("");
  }
}

void DocumentationBuilder::_add(const Key &key,
                                const Translateable::TranslateablePtr &value) {
  _components[key] = value;
}

bool DocumentationBuilder::_hasKey(const Key &key) const {
  return _components.count(key);
}
