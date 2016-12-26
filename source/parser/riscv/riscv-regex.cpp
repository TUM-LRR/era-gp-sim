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

#include "parser/riscv/riscv-regex.hpp"

#include <cctype>
#include "common/assert.hpp"
#include "parser/common/compile-error-list.hpp"

RiscvParser::RiscvRegex::RiscvRegex() {
}

static size_t trimRight(const std::string &line, size_t pos) {
  --pos;
  while (pos > 0 && std::isspace(line[pos])) {
    --pos;
  }
  ++pos;
  return pos;
}

// Returns true if successful
bool RiscvParser::RiscvRegex::_readInstructionOrLabel(
    const std::string &line,
    CodeCoordinate lineCoordinate,
    CompileErrorList &errors,
    size_t &pos) {
  // Skip spaces
  for (; pos < line.size() && std::isspace(line[pos]); pos++) {
  }

  size_t startPos = pos;
  // Add valid characters to _instruction
  for (; pos < line.size() &&
         (std::isalnum(line[pos]) || line[pos] == '_' || line[pos] == '.');
       ++pos) {
  }

  // If we read through the whole string, the instruction is finished
  if (pos >= line.size()) {
    _instruction = _getPositionedString(
        line, lineCoordinate, errors, startPos, line.size());
    return true;
  }

  // If we hit a colon, its a label
  if (line[pos] == ':') {
    // If a label is already defined, add an error
    if (!_label.string().empty()) {
      errors.pushError(_getCharacterPosition(lineCoordinate, errors, pos),
                       "Multiple labels per line aren't allowed!");
      return false;
    }

    _label = _getPositionedString(line, lineCoordinate, errors, startPos, pos);
    ++pos;
    return _readInstructionOrLabel(line, lineCoordinate, errors, pos);
  }

  // If we hit a space or a comment start, the instruction is finished
  if (std::isspace(line[pos]) || line[pos] == ';') {
    _instruction =
        _getPositionedString(line, lineCoordinate, errors, startPos, pos);
    return true;
  }

  // Otherwise we hit an invalid character
  errors.pushError(_getCharacterPosition(lineCoordinate, errors, pos),
                   "Invalid character in instruction name!");
  return false;
}

// Returns true if successful
bool RiscvParser::RiscvRegex::_readParameter(const std::string &line,
                                             CodeCoordinate lineCoordinate,
                                             CompileErrorList &errors,
                                             size_t &pos) {
  // Saves if we're inside a string
  bool quoted = false;

  // Skip spaces
  for (; pos < (size_t)line.size() && std::isspace(line[pos]); pos++) {
  }

  size_t startPos = pos;
  size_t endPos = pos;
  // Add characters to parameter until we hit a ',' outside of quotes
  for (; pos < line.size(); ++pos, ++endPos) {
    if (line[pos] == '"' && line[pos - 1] != '\\') {
      quoted = !quoted;
    }
    if (line[pos] == ',' && !quoted) {
      ++pos;
      break;
    } else if (line[pos] == ';' && !quoted) {
      break;
    }
  }

  size_t trimmedPos = trimRight(line, endPos);

  if (startPos < trimmedPos) {
    _parameters.push_back(_getPositionedString(
        line, lineCoordinate, errors, startPos, trimmedPos));
  }

  return true;
}

void RiscvParser::RiscvRegex::_resetResults() {
  _label = PositionedString();
  _instruction = PositionedString();
  _parameters.clear();
  _isValid = false;
}

size_t RiscvParser::RiscvRegex::matchLine(const std::string &line,
                                          CodeCoordinate lineCoordinate,
                                          CompileErrorList &errors) {
  size_t pos = 0;

  _resetResults();

  // Read instruction and label
  if (!_readInstructionOrLabel(line, lineCoordinate, errors, pos)) {
    return pos;
  }

  // Read parameters until it fails or we reached the end of the line or a
  // comment
  while (pos < line.size() && line[pos] != ';') {
    if (!_readParameter(line, lineCoordinate, errors, pos)) {
      return pos;
    }
  }

  _isValid = true;
  return pos;
}

bool RiscvParser::RiscvRegex::isValid() const noexcept {
  return _isValid;
}

bool RiscvParser::RiscvRegex::hasLabel() const noexcept {
  return !_label.string().empty();
}

bool RiscvParser::RiscvRegex::hasInstruction() const noexcept {
  return !_instruction.string().empty();
}

bool RiscvParser::RiscvRegex::isDirective() const {
  return hasInstruction() && _instruction.string()[0] == '.';
}

const PositionedString &RiscvParser::RiscvRegex::getLabel() const noexcept {
  return _label;
}

PositionedString RiscvParser::RiscvRegex::getInstruction() const {
  auto instruction = _instruction;

  if (isDirective()) {
    instruction = PositionedString(instruction.string().substr(1),
                                   instruction.positionInterval());
  }

  return instruction;
}

const PositionedString &RiscvParser::RiscvRegex::getParameter(int n) const {
  assert::that(n >= 0 && n < getParameterCount());
  return _parameters[n];
}

int RiscvParser::RiscvRegex::getParameterCount() const noexcept {
  return _parameters.size();
}

CodePositionInterval
RiscvParser::RiscvRegex::_getCharacterPosition(CodeCoordinate lineCoordinate,
                                               CompileErrorList &errors,
                                               size_t pos) const {
  CodePosition position(lineCoordinate, pos);
  return CodePositionInterval(position, position >> 1);
}

CodePositionInterval
RiscvParser::RiscvRegex::_getCharacterInterval(CodeCoordinate lineCoordinate,
                                               CompileErrorList &errors,
                                               size_t start,
                                               size_t end) const {
  CodePosition nstart(lineCoordinate, start);
  CodePosition nend(lineCoordinate, end);
  return CodePositionInterval(nstart, nend);
}

PositionedString
RiscvParser::RiscvRegex::_getPositionedString(const std::string &line,
                                              CodeCoordinate lineCoordinate,
                                              CompileErrorList &errors,
                                              size_t start,
                                              size_t end) const {
  return PositionedString(
      line.substr(start, end - start),
      _getCharacterInterval(lineCoordinate, errors, start, end));
}
