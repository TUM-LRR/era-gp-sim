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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_TRANSFORMATION_PARAMETERS_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_TRANSFORMATION_PARAMETERS_HPP

#include "arch/common/architecture.hpp"
#include "parser/independent/memory-allocator.hpp"
#include "parser/independent/syntax-tree-generator.hpp"

class TransformationParameters {
 public:
  TransformationParameters(const Architecture& architecture,
                           const MemoryAllocator& allocator,
                           const SyntaxTreeGenerator& generator);
  const Architecture& architecture() const noexcept;
  const MemoryAllocator& allocator() const noexcept;
  const SyntaxTreeGenerator& generator() const noexcept;

 private:
  Architecture _architecture;
  MemoryAllocator _allocator;
  SyntaxTreeGenerator _generator;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_TRANSFORMATION_PARAMETERS_HPP */
