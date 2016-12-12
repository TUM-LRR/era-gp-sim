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

#include "parser/compile-error-list.hpp"

const std::vector<CompileError> errors() const noexcept{
    return _errors;
}

static bool existsError(const std::vector<CompileError>& errors, CompileErrorSeverity severity)
{
    return std::any_of(errors.begin(), errors.end(), [] (const CompileError& error) {
            return error.severity() == severity;
        });
}

    bool hasErrors() const {
        return existsError(_errors, CompileErrorSeverity::ERROR);
    }
    bool hasWarnings() const{
        return existsError(_errors, CompileErrorSeverity::ERROR);
    }
    bool hasInformation() const{
        return existsError(_errors, CompileErrorSeverity::ERROR);
    }
    void add(const CompileError& error){
        _errors.push_back(error);
    }
    void add(const std::string& message, const CodePositionInterval& interval, CompileErrorSeverity severity){
        _errors.push_back(CompileError(message, interval, severity));
    }
