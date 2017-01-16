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

#include "common/status.hpp"

#include <string>

Status Status::Fail(const std::string& message) {
  return {Status::FAILURE, message};
}

Status::Status(Code code, const std::string& message)
: _code(code), _message(message) {
}

bool Status::operator==(const Status& other) const noexcept {
  return *this == other._code;
}

bool operator==(const Status& status, Status::Code code) noexcept {
  return status._code == code;
}

bool operator==(Status::Code code, const Status& status) noexcept {
  return status == code;
}

const Status::Code& Status::code() const noexcept {
  return _code;
}

const std::string& Status::message() const noexcept {
  return _message;
}

Status::operator bool() const noexcept {
  return _code == Code::OK;
}
