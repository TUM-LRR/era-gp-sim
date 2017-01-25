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

#include "parser/independent/execute-immutable-arguments.hpp"

#include "parser/independent/enhance-symbol-table-immutable-arguments.hpp"
#include "parser/independent/preprocessing-immutable-arguments.hpp"

// Just constructors and getters.

ExecuteImmutableArguments::ExecuteImmutableArguments(
    const Architecture& architecture,
    const SyntaxTreeGenerator& generator,
    const MemoryAllocator& allocator,
    const SymbolReplacer& replacer)
: _architecture(architecture)
, _generator(generator)
, _allocator(allocator)
, _replacer(replacer) {
}
ExecuteImmutableArguments::ExecuteImmutableArguments(
    const PreprocessingImmutableArguments& beforeBeforePass,
    const MemoryAllocator& allocator,
    const SymbolReplacer& replacer)
: ExecuteImmutableArguments(beforeBeforePass.architecture(),
                            beforeBeforePass.generator(),
                            allocator,
                            replacer) {
}
ExecuteImmutableArguments::ExecuteImmutableArguments(
    const EnhanceSymbolTableImmutableArguments& beforePass,
    const SymbolReplacer& replacer)
: ExecuteImmutableArguments(beforePass.architecture(),
                            beforePass.generator(),
                            beforePass.allocator(),
                            replacer) {
}
const Architecture& ExecuteImmutableArguments::architecture() const noexcept {
  return _architecture;
}
const SymbolReplacer& ExecuteImmutableArguments::replacer() const noexcept {
  return _replacer;
}
const SyntaxTreeGenerator& ExecuteImmutableArguments::generator() const
    noexcept {
  return _generator;
}
const MemoryAllocator& ExecuteImmutableArguments::allocator() const noexcept {
  return _allocator;
}
