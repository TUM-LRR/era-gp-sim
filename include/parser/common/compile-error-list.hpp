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

#ifndef ERAGPSIM_PARSER_COMPILE_ERROR_LIST_HPP
#define ERAGPSIM_PARSER_COMPILE_ERROR_LIST_HPP

#include <functional>
#include <vector>
#include "parser/common/code-position-interval.hpp"
#include "parser/common/compile-error-severity.hpp"
#include "parser/common/compile-error.hpp"

#define addError(interval, message, ...) \
  addErrorInternal(                      \
      (interval), QT_TRANSLATE_NOOP("Parser Errors", message), {__VA_ARGS__})
#define addWarning(interval, message, ...)                          \
  addWarningInternal((interval),                                    \
                     QT_TRANSLATE_NOOP("Parser Warnings", message), \
                     {__VA_ARGS__})
#define addInformation(interval, message, ...)                             \
  addInformationInternal((interval),                                       \
                         QT_TRANSLATE_NOOP("Parser Information", message), \
                         {__VA_ARGS__})
#define addCompileError(severity, interval, message, ...)                   \
  addCompileErrorInternal((severity),                                       \
                          (interval),                                       \
                          QT_TRANSLATE_NOOP("Parser Information", message), \
                          {__VA_ARGS__})

class CompileErrorList {
 public:
  CompileErrorList() = default;

  const CompileErrorVector errors() const noexcept;
  bool hasErrors() const;
  bool hasWarnings() const;
  bool hasInformation() const;
  bool empty() const;
  std::size_t errorCount() const;
  std::size_t warningCount() const;
  std::size_t informationCount() const;
  std::size_t size() const;
  void addRaw(const CompileError& error);
  void
  addCompileErrorInternal(CompileErrorSeverity severity,
                          const CodePositionInterval& interval,
                          const char* message,
                          const std::initializer_list<std::string>& parameters);
  void addErrorInternal(const CodePositionInterval& interval,
                        const char* message,
                        const std::initializer_list<std::string>& parameters);
  void addWarningInternal(const CodePositionInterval& interval,
                          const char* message,
                          const std::initializer_list<std::string>& parameters);
  void
  addInformationInternal(const CodePositionInterval& interval,
                         const char* message,
                         const std::initializer_list<std::string>& parameters);

 private:
  CompileErrorVector _errors;
};

#endif /* ERAGPSIM_PARSER_COMPILE_ERROR_LIST_HPP */
