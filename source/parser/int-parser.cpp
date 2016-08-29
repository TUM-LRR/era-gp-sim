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

#include <cctype>
#include <limits>
#include <stdexcept>

static void addError(const std::string &message, CompileState &state) {
  state.errorList.push_back(
      CompileError{message, state.position, CompileErrorSeverity::ERROR});
}

template <typename InputT, typename OutputT>
static bool
parseIntegerInternal(const InputT &input,
                     OutputT &output,
                     CompileState &state,
                     OutputT (*func)(const InputT &, std::size_t *, int)) {
  OutputT result;
  std::size_t parsed_length;
  bool success = false;

  // Preventing spaces at the start the of string.
  if (std::isspace(static_cast<unsigned char>(input[0]))) {
    addError("Not a valid integer.", state);
    return false;
  }

  // Preventing negative integers for unsigned parsing, because std::stoul
  // doesn't fail on negative integers.
  if (!std::numeric_limits<OutputT>::is_signed &&
      static_cast<unsigned char>(input[0]) == '-') {
    addError("Integer out of range.", state);
    return false;
  }

  try {
    result = func(input, &parsed_length, 0);
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

bool IntParser::parseInteger(const std::string &input,
                             int &output,
                             CompileState &state) {
  return parseIntegerInternal(input, output, state, std::stoi);
}

bool IntParser::parseInteger(const std::string &input,
                             long &output,
                             CompileState &state) {
  return parseIntegerInternal(input, output, state, std::stol);
}

bool IntParser::parseInteger(const std::string &input,
                             long long &output,
                             CompileState &state) {
  return parseIntegerInternal(input, output, state, std::stoll);
}

bool IntParser::parseInteger(const std::string &input,
                             unsigned long &output,
                             CompileState &state) {
  return parseIntegerInternal(input, output, state, std::stoul);
}

bool IntParser::parseInteger(const std::string &input,
                             unsigned long long &output,
                             CompileState &state) {
  return parseIntegerInternal(input, output, state, std::stoull);
}
