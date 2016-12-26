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

#ifndef ERAGPSIM_PARSER_FINAL_REPRESENTATION_HPP
#define ERAGPSIM_PARSER_FINAL_REPRESENTATION_HPP

#include <memory>
#include <unordered_map>
#include <vector>
#include "parser/common/compile-error-list.hpp"
#include "parser/common/compile-error.hpp"
#include "parser/common/final-command.hpp"
#include "parser/common/macro-information.hpp"

/**
 * \brief Denotes the temporary output of an IntermediateRepresentator ready to
 * be used by the architecture.
 */
class FinalRepresentation {
 public:
  /**
   * \brief Helper type for address mapping from memory address to instruction
   * index in the command vector.
   */
  using AddressMapping = std::unordered_map<MemoryAddress, std::size_t>;

  /**
   * \brief Creates an empty final representation.
   */
  FinalRepresentation() = default;

  /**
   * \brief Creates a final representation with the given arguments.
   * \param commandList The list with the assembled instructions and their
   * location in code.
   * \param errorList The list with errors that occurred during the compilation
   * process.
   * \param macroList A helper list containing all macros which have been
   * assembled.
   */
  FinalRepresentation(const FinalCommandVector& commandList,
                      const CompileErrorList& errorList,
                      const MacroInformationVector& macroList);

  /**
   * \brief Returns the list with the assembled instructions and their location
   * in code.
   * \return The list with the assembled instructions and their location in
   * code.
   */
  const FinalCommandVector& commandList() const noexcept;

  /**
   * \brief Returns the list with errors that occurred during the compilation
   * process.
   * \return The list with errors that occurred during the compilation process.
   */
  const CompileErrorList& errorList() const noexcept;

  /**
   * \brief Returns a helper list containing all macros which have been
   * assembled.
   * \return A helper list containing all macros which have been assembled.
   */
  const MacroInformationVector& macroList() const noexcept;

  /**
   * \brief Create a mapping between index in memory and index in command vector
   * of this FinalRepresentation.
   * \return The created mapping.
   */
  AddressMapping createMapping() const;

 private:
  /**
   * \brief The list with the assembled instructions and their location in code.
   */
  FinalCommandVector _commandList;

  /**
   * \brief The list with errors that occurred during the compilation process.
   */
  CompileErrorList _errorList;

  /**
   * \brief A helper list containing all macros which have been assembled.
   */
  MacroInformationVector _macroList;
};

#endif
