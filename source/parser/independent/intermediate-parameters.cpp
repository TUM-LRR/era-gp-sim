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

#include "parser/independent/intermediate-parameters.hpp"

PreprocessingImmutableArguments::PreprocessingImmutableArguments(
    const Architecture& architecture, const SyntaxTreeGenerator& generator)
: _architecture(architecture), _generator(generator) {
}
const Architecture& PreprocessingImmutableArguments::architecture() const
    noexcept {
  return _architecture;
}
const SyntaxTreeGenerator& PreprocessingImmutableArguments::generator() const
    noexcept {
  return _generator;
}

EnhanceSymbolTableImmutableArguments::EnhanceSymbolTableImmutableArguments(
    const Architecture& architecture,
    const SyntaxTreeGenerator& generator,
    const MemoryAllocator& allocator)
: _architecture(architecture), _generator(generator), _allocator(allocator) {
}
EnhanceSymbolTableImmutableArguments::EnhanceSymbolTableImmutableArguments(
    const PreprocessingImmutableArguments& beforePass,
    const MemoryAllocator& allocator)
: EnhanceSymbolTableImmutableArguments(
      beforePass.architecture(), beforePass.generator(), allocator) {
}
const Architecture& EnhanceSymbolTableImmutableArguments::architecture() const
    noexcept {
  return _architecture;
}
const SyntaxTreeGenerator&
EnhanceSymbolTableImmutableArguments::generator() const noexcept {
  return _generator;
}
const MemoryAllocator& EnhanceSymbolTableImmutableArguments::allocator() const
    noexcept {
  return _allocator;
}

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

TransformationParameters::TransformationParameters(
    const Architecture& architecture,
    const MemoryAllocator& allocator,
    const SyntaxTreeGenerator& generator)
: _architecture(architecture), _allocator(allocator), _generator(generator) {
}
const Architecture& TransformationParameters::architecture() const noexcept {
  return _architecture;
}
const MemoryAllocator& TransformationParameters::allocator() const noexcept {
  return _allocator;
}
const SyntaxTreeGenerator& TransformationParameters::generator() const
    noexcept {
  return _generator;
}
