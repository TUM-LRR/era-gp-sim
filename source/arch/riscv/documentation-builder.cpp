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
#include "common/assert.hpp"

using Key = DocumentationBuilder::Key;

const std::vector<std::string> DocumentationBuilder::_colors = {
    //blue, red, orange/brown
    "#3366c4", "#66c433", "#c46633"};

DocumentationBuilder::DocumentationBuilder() : _operandCount(0) {}

Translateable DocumentationBuilder::build() {
  assert::that(_hasKey(Key::INSTRUCTION));
  assert::that(_hasKey(Key::S_SYNTAX));
  return Translateable(
      "<b>%1</b>: <code>%2</code><p style=\"margin-left:5%\">%3</p>%4<br>%5",
      {_components[Key::INSTRUCTION], _components[Key::S_SYNTAX],
       _optional(Key::OPERAND_DESC), _optional(Key::S_DESC),
       _optional(Key::D_DESC)});
}

DocumentationBuilder &DocumentationBuilder::instruction(const std::string &s) {
  _add(Key::INSTRUCTION, std::make_shared<Translateable>(s));
  return *this;
}

DocumentationBuilder &DocumentationBuilder::detailDescription(
    const std::string &s) {
  _add(Key::D_DESC, std::make_shared<Translateable>(s));
  return *this;
}

DocumentationBuilder& DocumentationBuilder::detailDescription(const Translateable::TranslateablePtr &msg) {
    _add(Key::D_DESC, msg);
    return *this;
}

DocumentationBuilder &DocumentationBuilder::operandDescription(
    const std::string &name, const std::string &description) {
  auto it = _components.find(Key::OPERAND_DESC);
  auto &existingTranslateable = (it != _components.end())
                                    ? (*it).second
                                    : _components[Key::OPERAND_DESC] =
                                          std::make_shared<Translateable>("");
  auto &baseString = existingTranslateable->getModifiableBaseString();
  baseString += '%';
  baseString += std::to_string(_operandCount + 1);  // will be %1, %2, ...
  //all template arguments are explicitly given because gcc has a hard time guessing
  auto operand = std::make_shared<Translateable, std::string, std::initializer_list<const std::string>>(
      "<br><code><span style=\"color:%1\"><b>%2</b></span></code>: %3",
      {_nextColor(), name, description});
  existingTranslateable->addOperand(operand);
  ++_operandCount;
  return *this;
}

DocumentationBuilder &DocumentationBuilder::shortDescription(
    const std::string &s) {
  _add(Key::S_DESC, std::make_shared<Translateable>(s));
  return *this;
}

DocumentationBuilder &DocumentationBuilder::shortSyntax(
    std::initializer_list<const std::string> operands) {
  auto translateable = std::make_shared<Translateable>("");
  std::string result = "";
  auto i = 0;
  for (const auto &op : operands) {
    result += "<span style=\"color:%";
    result += std::to_string((2 * i) + 1);  // will be %1, %3, ...
    result += "\"><b>%";
    result += std::to_string(2 + 2 * i);  // will be %2, %4, ...
    result += "</b><span>, ";
    translateable->addOperand(
        std::make_shared<Translateable>(_colors[i % _colors.size()]));
    translateable->addOperand(std::make_shared<Translateable>(op));
    ++i;
  }

  // remove last ", " and put into translateable
  translateable->getModifiableBaseString() +=
      result.substr(0, result.size() - 2);
  _add(Key::S_SYNTAX, translateable);
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
