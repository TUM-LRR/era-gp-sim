/*
* ERASIM Assembler Interpreter
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

#ifndef ERASIM_PARSER_COMMON_COMPILE_ERROR_LIST_HPP
#define ERASIM_PARSER_COMMON_COMPILE_ERROR_LIST_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "common/translateable.hpp"
#include "parser/common/code-position-interval.hpp"
#include "parser/common/compile-error-severity.hpp"
#include "parser/common/compile-error.hpp"

// These macros simplify adding compile errors to the compile error list. The
// macros embed the QT_TRANSLATE macro, so it is not needed to enclose the
// message when using the macros.
#define pushError(interval, message, ...) \
  pushErrorInternal(                      \
      (interval), QT_TRANSLATE_NOOP("Parser Errors", message), ##__VA_ARGS__)
#define pushWarning(interval, message, ...)                          \
  pushWarningInternal((interval),                                    \
                      QT_TRANSLATE_NOOP("Parser Warnings", message), \
                      ##__VA_ARGS__)
#define pushInformation(interval, message, ...)                             \
  pushInformationInternal((interval),                                       \
                          QT_TRANSLATE_NOOP("Parser Information", message), \
                          ##__VA_ARGS__)
#define pushCompileError(severity, interval, message, ...)                   \
  pushCompileErrorInternal((severity),                                       \
                           (interval),                                       \
                           QT_TRANSLATE_NOOP("Parser Information", message), \
                           ##__VA_ARGS__)
/**
 * A list of compile errors, enhanced by some helper methods.
 */
class CompileErrorList {
 public:
  using size_t = std::size_t;

  /**
   * \return The internal compile error vector.
   */
  const CompileErrorVector& errors() const noexcept;

  /**
   * A helper method to determine if the list has any errors (not
   * including warnings, information).
   *
   * \return True, if errors exist, else false.
   */
  bool hasErrors() const noexcept;

  /**
   * A helper method to determine if the list has any warning (not
   * including error, information).
   *
   * \return True, if warnings exist, else false.
   */
  bool hasWarnings() const noexcept;

  /**
   * A helper method to determine if the list has any information entries
   * (not including warnings, information).
   *
   * \return True, if information entires exist, else false.
   */
  bool hasInformation() const noexcept;

  /**
   * Forwarded method from the internal vector. Determines, if there are
   * any compile errors (errors, warnings, information) in the list.
   *
   * \return True, if the vector is empty, else false.
   */
  bool empty() const noexcept;

  /**
   * A helper method to determine if the number of errors (not including
   * warnings, information).
   *
   * \return The number of errors.
   */
  size_t errorCount() const noexcept;

  /**
   * A helper method to determine if the number of warnings (not
   * including errors, information).
   *
   * \return The number of warnings.
   */
  size_t warningCount() const noexcept;

  /**
   * A helper method to determine if the number of information entries
   * (not including warnings, information).
   *
   * \return The number of information entries.
   */
  size_t informationCount() const noexcept;

  /**
   * Forwarded method from the internal vector. Determines, the number of
   * compile errors (errors, warnings, information) in the list.
   *
   * \return The number of all compile errors in the list.
   */
  size_t size() const noexcept;

  /**
   * Adds a compile error class instance to the list.
   * \param error The given compile error to insert.
   */
  void addRaw(const CompileError& error);

  /**
   * Adds a compile error created from the arguments to the list.
   *
   * \param severity The given severity of the compile error.
   * \param interval The given code position interval where to error occured.
   * \param message The message to record. (do not change to std::string as
   * input parameter! We want to force that people insert a string literal here,
   * for translation using the QT_TRANSLATE_NOOP macro).
   * \param parameters The parameters for the message.
   */
  template <typename... Args>
  void pushCompileErrorInternal(CompileErrorSeverity severity,
                                const CodePositionInterval& interval,
                                const char* message,
                                Args&&... parameters) {
    addRaw({std::make_shared<Translateable>(message,
                                            std::forward<Args>(parameters)...),
            interval,
            severity});
  }

  /**
   * Adds a compile error created from the arguments to the list,
   * pre-specified of severity 'error'.
   *
   * \param interval The given code position interval where to error occured.
   * \param message The message to record. (do not change to std::string as
   * input parameter! We want to force that people insert a string literal here,
   * for translation using the QT_TRANSLATE_NOOP macro).
   * \param parameters The parameters for the message.
   */
  template <typename... Args>
  void pushErrorInternal(const CodePositionInterval& interval,
                         const char* message,
                         Args&&... parameters) {
    pushCompileError(CompileErrorSeverity::ERROR,
                     interval,
                     message,
                     std::forward<Args>(parameters)...);
  }

  /**
   * Adds a compile error created from the arguments to the list,
   * pre-specified of severity 'warning'.
   *
   * \param interval The given code position interval where to error occured.
   * \param message The message to record. (do not change to std::string as
   * input parameter! We want to force that people insert a string literal here,
   * for translation using the QT_TRANSLATE_NOOP macro).
   * \param parameters The parameters for the message.
   */
  template <typename... Args>
  void pushWarningInternal(const CodePositionInterval& interval,
                           const char* message,
                           Args&&... parameters) {
    pushCompileError(CompileErrorSeverity::WARNING,
                     interval,
                     message,
                     std::forward<Args>(parameters)...);
  }

  /**
   * Adds a compile error created from the arguments to the list,
   * pre-specified of severity 'information'.
   *
   * \param interval The given code position interval where to error occured.
   * \param message The message to record. (do not change to std::string as
   * input parameter! We want to force that people insert a string literal here,
   * for translation using the QT_TRANSLATE_NOOP macro).
   * \param parameters The parameters for the message.
   */
  template <typename... Args>
  void pushInformationInternal(const CodePositionInterval& interval,
                               const char* message,
                               Args&&... parameters) {
    pushCompileError(CompileErrorSeverity::INFORMATION,
                     interval,
                     message,
                     std::forward<Args>(parameters)...);
  }

 private:
  /** The internal vector for compile errors. */
  CompileErrorVector _errors;
};

#endif /* ERASIM_PARSER_COMMON_COMPILE_ERROR_LIST_HPP */
