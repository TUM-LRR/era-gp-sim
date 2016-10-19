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

#ifndef ERAGPSIM_PARSER_RISCV_DIRECTIVES_HPP
#define ERAGPSIM_PARSER_RISCV_DIRECTIVES_HPP

#include "memory-definition-directive.hpp"
#include "memory-reservation-directive.hpp"
#include "section-directive.hpp"
#include "macro-directive.hpp"
#include "macro-end-directive.hpp"
#include "constant-directive.hpp"

/**
 * \brief A helper class for all riscv-specific macro definitions.
 */
class RiscvDirectives {
 public:
  template <typename T>
  static MemoryDefinitionDirective<T>
  createMemoryDefinitionDirective(const LineInterval& lines,
                                  const std::vector<std::string>& labels,
                                  const std::string& name,
                                  std::size_t cellSize,
                                  const std::vector<std::string>& values) {
    return MemoryDefinitionDirective<T>(
        lines, labels, name, cellSize, values, _processValues<T>);
  }

  /**
   * \brief Creates a memory definition directive.
   */
  template <typename T>
  static MemoryDefinitionDirective<T>
  createMemoryDefinitionDirective(const LineInterval& lines,
                                  const std::vector<std::string>& labels,
                                  const std::string& name,
                                  const std::vector<std::string>& values) {
    return MemoryDefinitionDirective<T>(
        lines, labels, name, values, _processValues<T>);
  }

  /**
    * \brief Creates a memory definition directive of 8-bit values.
    */
  static MemoryDefinitionDirective<uint8_t>
  createMemoryDefinitionI8Directive(const LineInterval& lines,
                                    const std::vector<std::string>& labels,
                                    const std::string& name,
                                    const std::vector<std::string>& values);

  /**
    * \brief Creates a memory definition directive of 16-bit values.
    */
  static MemoryDefinitionDirective<uint16_t>
  createMemoryDefinitionI16Directive(const LineInterval& lines,
                                     const std::vector<std::string>& labels,
                                     const std::string& name,
                                     const std::vector<std::string>& values);

  /**
    * \brief Creates a memory definition directive of 32-bit values.
    */
  static MemoryDefinitionDirective<uint32_t>
  createMemoryDefinitionI32Directive(const LineInterval& lines,
                                     const std::vector<std::string>& labels,
                                     const std::string& name,
                                     const std::vector<std::string>& values);

  /**
    * \brief Creates a memory definition directive of 64-bit values.
    */
  static MemoryDefinitionDirective<uint64_t>
  createMemoryDefinitionI64Directive(const LineInterval& lines,
                                     const std::vector<std::string>& labels,
                                     const std::string& name,
                                     const std::vector<std::string>& values);

  /**
   * \brief Creates a memory reservation directive.
   */
  static MemoryReservationDirective
  createMemoryReservationDirective(const LineInterval& lines,
                                   const std::vector<std::string>& labels,
                                   const std::string& name,
                                   std::size_t cellSize,
                                   const std::vector<std::string>& values);

  /**
    * \brief Creates a memory reservation directive of 8-bit values.
    */
  static MemoryReservationDirective
  createMemoryReservationI8Directive(const LineInterval& lines,
                                     const std::vector<std::string>& labels,
                                     const std::string& name,
                                     const std::vector<std::string>& values);

  /**
    * \brief Creates a memory reservation directive of 16-bit values.
    */
  static MemoryReservationDirective
  createMemoryReservationI16Directive(const LineInterval& lines,
                                      const std::vector<std::string>& labels,
                                      const std::string& name,
                                      const std::vector<std::string>& values);

  /**
    * \brief Creates a memory reservation directive of 32-bit values.
    */
  static MemoryReservationDirective
  createMemoryReservationI32Directive(const LineInterval& lines,
                                      const std::vector<std::string>& labels,
                                      const std::string& name,
                                      const std::vector<std::string>& values);

  /**
    * \brief Creates a memory reservation directive of 64-bit values.
    */
  static MemoryReservationDirective
  createMemoryReservationI64Directive(const LineInterval& lines,
                                      const std::vector<std::string>& labels,
                                      const std::string& name,
                                      const std::vector<std::string>& values);

  /**
  * \brief Creates a constant directive (forwarded).
  */
  static ConstantDirective
  createConstantDirective(const LineInterval& lines,
                          const std::vector<std::string>& labels,
                          const std::string& name,
                          const std::vector<std::string>& arguments);
  /**
    * \brief Creates a macro directive (forwarded).
    */
  static MacroDirective
  createMacroDirective(const LineInterval& lines,
                       const std::vector<std::string>& labels,
                       const std::string& name,
                       const std::string& macroName,
                       const std::vector<std::string>& macroParameters);

  /**
    * \brief Creates a macro finish directive (forwarded).
    */
  static MacroEndDirective
  createMacroEndDirective(const LineInterval& lines,
                          const std::vector<std::string>& labels,
                          const std::string& name);

  /**
    * \brief Creates a section directive (forwarded).
    */
  static SectionDirective
  createSectionDirective(const LineInterval& lines,
                         const std::vector<std::string>& labels,
                         const std::string& name,
                         const std::string& section);

 private:
  /**
   * \brief The riscv parse function for arguments of a memory definition
   * directive.
   */
  template <typename T>
  const static typename MemoryDefinitionDirective<T>::ProcessValuesFunction
      _processValues;

  /**
   * \brief The riscv function for compiling reserve memory arguments.
   */
  static MemoryReservationDirective::ArgumentCompileFunction _argumentCompile;
};

template <typename T>
const typename MemoryDefinitionDirective<T>::ProcessValuesFunction
    RiscvDirectives::_processValues<T> =
        [](const std::vector<std::string>& values,
           std::size_t cellSize,
           CompileState& state,
           const std::function<void(T, std::size_t)>& handler) -> std::size_t {
  std::size_t currentPosition = 0;
  ExpressionCompiler<T> compiler =
      CLikeExpressionCompilers::createCLikeCompiler<T>();

  for (const auto& i : values) {
    if (i.empty()) {
      // Empty arguments are not allowed.
      state.addError("Argument is empty.");
    } else if (i.at(0) == '\"') {
      // It is a string if it begins with a "
      std::vector<T> temporaryData;
      if (StringParser::parseString(i, temporaryData, state)) {
        // We add each character of the string to our output.
        for (const auto& j : temporaryData) {
          handler(j, currentPosition);
          currentPosition += cellSize;
        }

        // Final 0 byte/character.
        handler(0, currentPosition);
        currentPosition += cellSize;
      }
    } else {
      // If it is no a string, we regularly parse it.
      T returnData = compiler.compile(i, state);
      handler(returnData, currentPosition);
      currentPosition += cellSize;
    }
  }

  // We return our size estimate.
  return currentPosition;
};

#endif /* ERAGPSIM_PARSER_RISCV_DIRECTIVES_HPP */
