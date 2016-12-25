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
  using AddressMapping = std::unordered_map<MemoryAddress, std::size_t>;

  FinalRepresentation() = default;
  FinalRepresentation(const FinalCommandVector& commandList,
                      const CompileErrorList& errorList,
                      const MacroInformationVector& macroList);
  const FinalCommandVector& commandList() const noexcept;
  const CompileErrorList& errorList() const noexcept;
  const MacroInformationVector& macroList() const noexcept;
  AddressMapping createMapping() const;

 private:
  FinalCommandVector _commandList;
  CompileErrorList _errorList;
  MacroInformationVector _macroList;
};

#endif
