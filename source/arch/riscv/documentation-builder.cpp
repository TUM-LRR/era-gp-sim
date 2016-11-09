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
    "#3366c4", "#66c433", "#c46633"};

DocumentationBuilder::DocumentationBuilder() : _operandCount(0) {}

std::string DocumentationBuilder::build() {
  assert::that(_hasKey(Key::INSTRUCTION));
  assert::that(_hasKey(Key::S_SYNTAX));
  std::string result = "<b>";
  result += _components[Key::INSTRUCTION];
  result += "</b>: <code>";
  result += _components[Key::S_SYNTAX];
  result += "</code>";
  _optionalPut(result, Key::OPERAND_DESC, "<p style=\"margin-left: 5%\">",
               "</p>");
  _optionalPut(result, Key::S_DESC);
  _optionalPut(result, Key::D_DESC, "<br>");
  //result += "</html>";
  return result;
}

DocumentationBuilder &DocumentationBuilder::instruction(const std::string &s) {
  _add(Key::INSTRUCTION, s);
  return *this;
}

DocumentationBuilder &DocumentationBuilder::detailDescription(
    const std::string &s) {
  _add(Key::D_DESC, s);
  return *this;
}

DocumentationBuilder &DocumentationBuilder::operandDescription(
    const std::string name, const std::string &description) {
  auto it = _components.find(Key::OPERAND_DESC);
  auto &existingOperandList = (it != _components.end())
                                  ? (*it).second
                                  : _components[Key::OPERAND_DESC] = "";

  existingOperandList += "<br><code><span style=\"color:";
  existingOperandList += _nextColor();
  existingOperandList += "\"><b>";
  existingOperandList += name;
  existingOperandList += "</b></span></code>: ";
  existingOperandList += description;
  ++_operandCount;
  return *this;
}

DocumentationBuilder &DocumentationBuilder::shortDescription(
    const std::string &s) {
  _add(Key::S_DESC, s);
  return *this;
}

DocumentationBuilder &DocumentationBuilder::shortSyntax(
    std::initializer_list<const std::string> operands) {
  std::string result = "";
  auto i = 0;
  for (const std::string &ops : operands) {
    result += "<span style=\"color:";
    result += _colors[i % _colors.size()];
    result += "\"><b>";
    result += ops;
    result += "</b></span>, ";
    ++i;
  }
  // remove last ", " and put into map
  _add(Key::S_SYNTAX, result.substr(0, result.size() - 2));
  return *this;
}

const std::string &DocumentationBuilder::_nextColor() const {
  return _colors[_operandCount % _colors.size()];
}

void DocumentationBuilder::_optionalPut(std::string &string, const Key &key,
                                        const std::string &start,
                                        const std::string &end) {
  if (_hasKey(key)) {
    string += start;
    string += _components[key];
    string += end;
  }
}
