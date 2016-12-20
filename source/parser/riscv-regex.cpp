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

#include "parser/riscv-regex.hpp"

#include <cctype>
#include "common/assert.hpp"
#include "parser/compile-error-annotator.hpp"

RiscvParser::RiscvRegex::RiscvRegex() {}

static void trimRight(std::string &str) {
  int pos = str.size() - 1;

  while (pos >= 0 && std::isspace(str[pos])) pos--;

  pos++;

  if (pos < str.size() && pos >= 0) str.erase(pos);
}

// Returns true if successful
bool RiscvParser::RiscvRegex::_readInstructionOrLabel(const std::string &line,
                                                      const CompileErrorAnnotator& annotator,
                                                      size_t &pos) {
  // Skip spaces
  for (; pos < line.size() && std::isspace(line[pos]); pos++)
    ;

  // Add valid characters to _instruction
  for (; pos < line.size() &&
         (std::isalnum(line[pos]) || line[pos] == '_' || line[pos] == '.');
       pos++)
    _instruction.push_back(line[pos]);

  // If we read through the whole string, the instruction is finished
  if (pos >= line.size()) return true;

  // If we hit a colon, its a label
  if (line[pos] == ':') {
    // If a label is already defined, add an error
    if (_label.size() > 0) {
      annotator.addError(_getCharacterPosition(annotator, pos),
                      "Multiple labels per line aren't allowed!");
      return false;
    }

    _label = _instruction;
    _instruction.clear();
    pos++;
    return _readInstructionOrLabel(line, annotator, pos);
  }

  // If we hit a space or a comment start, the instruction is finished
  if (std::isspace(line[pos]) || line[pos] == ';') return true;

  // Otherwise we hit an invalid character
  annotator.addError(_getCharacterPosition(annotator, pos),
                  "Invalid character in instruction name!");
  return false;
}

// Returns true if successful
bool RiscvParser::RiscvRegex::_readParameter(const std::string &line,
                                             const CompileErrorAnnotator& annotator, size_t &pos) {
  // Saves if we're inside a string
  bool quoted = false;
  std::string parameter;

  // Skip spaces
  for (; pos < line.size() && std::isspace(line[pos]); pos++)
    ;

  // Add characters to parameter until we hit a ',' outside of quotes
  for (; pos < line.size(); pos++) {
    if (line[pos] == '"' && line[pos - 1] != '\\') {
      quoted = !quoted;
    }
    if (line[pos] == ',' && !quoted) {
      pos++;
      break;
    } else if (line[pos] == ';' && !quoted) {
      break;
    } else {
      parameter.push_back(line[pos]);
    }
  }

  trimRight(parameter);

  if (parameter.size() > 0) _parameters.push_back(parameter);

  return true;
}

void RiscvParser::RiscvRegex::_resetResults() {
  _label.clear();
  _instruction.clear();
  _parameters.clear();
  _isValid = false;
}

size_t RiscvParser::RiscvRegex::matchLine(const std::string &line,
                                        const CompileErrorAnnotator& annotator) {
  size_t pos = 0;

  _resetResults();

  // Read instruction and label
  if (!_readInstructionOrLabel(line, annotator, pos)) return pos;

  // Read parameters until it fails or we reached the end of the line or a
  // comment
  while (pos < line.size() && line[pos] != ';')
    if (!_readParameter(line, annotator, pos)) return pos;

  _isValid = true;
  return pos;
}

bool RiscvParser::RiscvRegex::isValid() { return _isValid; }

bool RiscvParser::RiscvRegex::hasLabel() { return _label.size() > 0; }

bool RiscvParser::RiscvRegex::hasInstruction() {
  return _instruction.size() > 0;
}

bool RiscvParser::RiscvRegex::isDirective() {
  return hasInstruction() && _instruction[0] == '.';
}

std::string RiscvParser::RiscvRegex::getLabel() { return _label; }

std::string RiscvParser::RiscvRegex::getInstruction() {
  std::string instruction = _instruction;

  if (instruction[0] == '.') {
    instruction.erase(0, 1);
  }

  return instruction;
}

std::string RiscvParser::RiscvRegex::getParameter(int n) {
  assert::that(n >= 0 && n < getParameterCount());
  return _parameters[n];
}

int RiscvParser::RiscvRegex::getParameterCount() { return _parameters.size(); }

CodePositionInterval RiscvParser::RiscvRegex::_getCharacterPosition(
    const CompileErrorAnnotator& annotator, size_t pos) const {
      CodePosition position (annotator.position().codePositionStart().line(), pos);
  return CodePositionInterval(position, position >> 1);
}
