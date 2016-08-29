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
#include "parser/int-parser.hpp"

#include <stdexcept>

static void addError(const std::string &message, CompileState &state) {
  state.errorList.push_back(
      CompileError{message, state.position, CompileErrorSeverity::ERROR});
}

bool IntParser::parseInteger(const std::string &input,
                             long long &output,
                             CompileState &state) {
  long long result;
  std::size_t parsed_length;
  bool success = false;

  try {
    result = std::stoll(input, &parsed_length, 0);
    if (parsed_length < input.length()) {
      addError("Not a valid integer.", state);
    } else {
      success = true;
    }
  } catch (const std::out_of_range &) {// Parsed integer is outside of range
    addError("Integer out of range.", state);
  } catch (const std::invalid_argument &) {// No conversion could be performed
    addError("Not a valid integer.", state);
  }

  if (success) {
    output = result;
  }

  return success;
}
