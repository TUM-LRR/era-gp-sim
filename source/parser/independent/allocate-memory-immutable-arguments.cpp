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

#include "parser/independent/allocate-memory-immutable-arguments.hpp"

#include "parser/independent/precompile-immutable-arguments.hpp"

// Just constructors and getters.

AllocateMemoryImmutableArguments::AllocateMemoryImmutableArguments(
    const Architecture& architecture,
    const SyntaxTreeGenerator& generator,
    const SymbolReplacer& preliminaryReplacer)
: _architecture(architecture)
, _generator(generator)
, _preliminaryReplacer(preliminaryReplacer) {
}

AllocateMemoryImmutableArguments::AllocateMemoryImmutableArguments(
    const PrecompileImmutableArguments& beforeBeforePass,
    const SymbolReplacer& preliminaryReplacer)
: AllocateMemoryImmutableArguments(beforeBeforePass.architecture(),
                                   beforeBeforePass.generator(),
                                   preliminaryReplacer) {
}

const Architecture& AllocateMemoryImmutableArguments::architecture() const
    noexcept {
  return _architecture;
}

const SyntaxTreeGenerator& AllocateMemoryImmutableArguments::generator() const
    noexcept {
  return _generator;
}

const SymbolReplacer&
AllocateMemoryImmutableArguments::preliminaryReplacer() const noexcept {
  return _preliminaryReplacer;
}
