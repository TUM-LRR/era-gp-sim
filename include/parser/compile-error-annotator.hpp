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

#ifndef ERAGPSIM_PARSER_COMPILE_ERROR_ANNOTATOR_HPP
#define ERAGPSIM_PARSER_COMPILE_ERROR_ANNOTATOR_HPP

class CompileError;
class CompileErrorList;
#include <QtGlobal>
#include <functional>
#include <string>
#include "parser/code-position-interval.hpp"
#include "parser/code-position.hpp"
#include "parser/compile-error-severity.hpp"

#define addError(interval, message, ...) \
  addErrorInternal(                      \
      (interval), QT_TRANSLATE_NOOP("Parser Errors", message), {__VA_ARGS__})
#define addErrorHere(message, ...)                                  \
  addErrorHereInternal(QT_TRANSLATE_NOOP("Parser Errors", message), \
                       {__VA_ARGS__})
#define addErrorDelta(first, second, message, ...)                   \
  addErrorDeltaInternal((first),                                     \
                        (second),                                    \
                        QT_TRANSLATE_NOOP("Parser Errors", message), \
                        {__VA_ARGS__})
#define addWarning(interval, message, ...)                          \
  addWarningInternal((interval),                                    \
                     QT_TRANSLATE_NOOP("Parser Warnings", message), \
                     {__VA_ARGS__})
#define addWarningHere(message, ...)                                    \
  addWarningHereInternal(QT_TRANSLATE_NOOP("Parser Warnings", message), \
                         {__VA_ARGS__})
#define addWarningDelta(first, second, message, ...)                     \
  addWarningDeltaInternal((first),                                       \
                          (second),                                      \
                          QT_TRANSLATE_NOOP("Parser Warnings", message), \
                          {__VA_ARGS__})
#define addInformation(interval, message, ...)                             \
  addInformationInternal((interval),                                       \
                         QT_TRANSLATE_NOOP("Parser Information", message), \
                         {__VA_ARGS__})
#define addInformationHere(message, ...)                                       \
  addInformationHereInternal(QT_TRANSLATE_NOOP("Parser Information", message), \
                             {__VA_ARGS__})
#define addInformationDelta(first, second, message, ...) \
  addInformationDeltaInternal(                           \
      (first),                                           \
      (second),                                          \
      QT_TRANSLATE_NOOP("Parser Information", message),  \
      {__VA_ARGS__})

class CompileErrorAnnotator {
 public:
  CompileErrorAnnotator(CompileErrorList& list,
                        const CodePositionInterval& position);
  CompileErrorAnnotator(const CompileErrorAnnotator& source,
                        const CodePositionInterval& position);
  CompileErrorAnnotator(CompileErrorList& list,
                        const CodePosition& start,
                        const CodePosition& end);
  CompileErrorAnnotator(const CompileErrorAnnotator& source,
                        const CodePosition& start,
                        const CodePosition& end);

  void
  addErrorInternal(const CodePositionInterval& interval,
                   const char* message,
                   const std::initializer_list<std::string>& parameters) const;
  void addErrorDeltaInternal(
      const CodePosition& deltaFirst,
      const CodePosition& deltaSecond,
      const char* message,
      const std::initializer_list<std::string>& parameters) const;
  void addErrorHereInternal(
      const char* message,
      const std::initializer_list<std::string>& parameters) const;
  void addWarningInternal(
      const CodePositionInterval& interval,
      const char* message,
      const std::initializer_list<std::string>& parameters) const;
  void addWarningDeltaInternal(
      const CodePosition& deltaFirst,
      const CodePosition& deltaSecond,
      const char* message,
      const std::initializer_list<std::string>& parameters) const;
  void addWarningHereInternal(
      const char* message,
      const std::initializer_list<std::string>& parameters) const;
  void addInformationInternal(
      const CodePositionInterval& interval,
      const char* message,
      const std::initializer_list<std::string>& parameters) const;
  void addInformationDeltaInternal(
      const CodePosition& deltaFirst,
      const CodePosition& deltaSecond,
      const char* message,
      const std::initializer_list<std::string>& parameters) const;
  void addInformationHereInternal(
      const char* message,
      const std::initializer_list<std::string>& parameters) const;

  CompileErrorList& errorList() const noexcept;
  const CodePositionInterval position() const noexcept;

 private:
  CompileErrorList& _list;
  CodePositionInterval _position;
};

#endif /* ERAGPSIM_PARSER_COMPILE_ERROR_ANNOTATOR_HPP */
