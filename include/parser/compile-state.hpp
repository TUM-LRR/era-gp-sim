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
/*
#ifndef ERAGPSIM_PARSER_COMPILE_STATE_HPP
#define ERAGPSIM_PARSER_COMPILE_STATE_HPP

#include <vector>

#include "parser/code-position-interval.hpp"
#include "parser/compile-error.hpp"
#include "parser/macro-directive-table.hpp"
#include "parser/parser-mode.hpp"

class MacroDirective;

/**
  * These macros simplifies adding compile errors to the compile state.
  * The macros embed the QT_TRANSLATE macro, so it is not needed to enclose the
 * message when using the macros
  *
#define addErrorHereT(msg, ...) \
  addErrorHere(QT_TRANSLATE_NOOP("Parser Error Messages", msg), ##__VA_ARGS__);
#define addErrorT(pos, msg, ...) \
  addError(pos, QT_TRANSLATE_NOOP("Parser Error Messages", msg), ##__VA_ARGS__);
#define addWarningHereT(msg, ...)                                   \
  addWarningHere(QT_TRANSLATE_NOOP("Parser Warning Messages", msg), \
                 ##__VA_ARGS__);
#define addWarningT(pos, msg, ...)                                   \
  addWarning(pos, QT_TRANSLATE_NOOP("Parser Warning Messages", msg), \
             ##__VA_ARGS__);
#define addInformationHereT(msg, ...)                                       \
  addInformationHere(QT_TRANSLATE_NOOP("Parser Information Messages", msg), \
                     ##__VA_ARGS__);
#define addInformationT(pos, msg, ...)                                       \
  addInformation(pos, QT_TRANSLATE_NOOP("Parser Information Messages", msg), \
                 ##__VA_ARGS__);

// Note: this class should be reworked and rebuilt to a full-grown class...

/**
 \brief Contains all relevant information for a compile run.
 *
struct CompileState {
  /**
   \brief Denotes the mode of the compiler.
   *
  ParserMode mode;

  /**
   \brief Equals the current position in code. TODO: Implement functionality.
   *
  CodePosition position;

  /**
   \brief Contains a list of compiler errors of this pass.
   *
  std::vector<CompileError> errorList;

  /**
   \brief Specifies the section the compiler is currently in.
   *
  std::string section = "text";

  /**
   * Saves registered macros.
   *
  MacroDirectiveTable macros;

  /**
   * \brief Adds an error to the state-internal error list at the current
   * position.
   * \param messageArgs The construction arguments for a Translateable, holding
   * the message
   *
<<<<<<< HEAD
  void addError(const std::string& message);
=======
  template <typename... Args>
  void addErrorHere(const Args&... messageArgs) {
    addError(position, messageArgs...);
  }
>>>>>>> master

  /**
   * \brief Adds an error to the state-internal error list.
   * \param position The position where the error occurred.
   * \param messageArgs The construction arguments for a Translateable, holding
   * the message
   *
<<<<<<< HEAD
  void addError(const std::string& message, const CodePosition& position);
=======
  template <typename... Args>
  void addError(const CodePosition& position, const Args&... messageArgs) {
    errorList.emplace_back(std::make_shared<Translateable>(messageArgs...),
                           position, CompileErrorSeverity::ERROR);
  }
>>>>>>> master

  /**
   * \brief Adds a warning to the state-internal error list at the current
   * position.
   * \param messageArgs The construction arguments for a Translateable, holding
   * the message
   *
<<<<<<< HEAD
  void addWarning(const std::string& message);
=======
  template <typename... Args>
  void addWarningHere(const Args&... messageArgs) {
    addWarning(position, messageArgs...);
  }
>>>>>>> master

  /**
   * \brief Adds a warning to the state-internal error list.
   * \param position The position where the warning occurred.
   * \param messageArgs The construction arguments for a Translateable, holding
   * the message
   *
<<<<<<< HEAD
  void addWarning(const std::string& message, const CodePosition& position);
=======
  template <typename... Args>
  void addWarning(const CodePosition& position, const Args&... messageArgs) {
    errorList.emplace_back(std::make_shared<Translateable>(messageArgs...),
                           position, CompileErrorSeverity::WARNING);
  }
>>>>>>> master

  /**
   * \brief Adds an information to the state-internal error list at the current
   * position.
   * \param messageArgs The construction arguments for a Translateable, holding
   * the message
   *
<<<<<<< HEAD
  void addInformation(const std::string& message);
=======
  template <typename... Args>
  void addInformationHere(const Args&... messageArgs) {
    addInformation(position, messageArgs...);
  }
>>>>>>> master

  /**
   * \brief Adds an information to the state-internal error list.
   * \param position The position where the information is needed.
   * \param messageArgs The construction arguments for a Translateable, holding
   * the message
   *
<<<<<<< HEAD
  void addInformation(const std::string& message, const CodePosition& position);
=======
  template <typename... Args>
  void addInformation(const CodePosition& position,
                      const Args&... messageArgs) {
    errorList.push_back(std::make_shared<Translateable>(messageArgs...),
                        position, CompileErrorSeverity::INFORMATION);
  }
>>>>>>> master

  /**
   * Registers a macro for compilation.
   * \param macro Macro to register.
   *
  void registerMacro(MacroDirective& macro);
};

#endif
