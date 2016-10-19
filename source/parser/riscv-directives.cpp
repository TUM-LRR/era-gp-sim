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

#include "parser\riscv-directives.hpp"

MemoryReservationDirective::ArgumentCompileFunction
    RiscvDirectives::_argumentCompile =
        [](const std::string& value, CompileState& state) -> std::size_t {
  return CLikeExpressionCompilers::CLikeCompilerU64.compile(value, state);
};

MemoryReservationDirective RiscvDirectives::createMemoryReservationDirective(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    std::size_t cellSize,
    const std::vector<std::string>& values) {
  return MemoryReservationDirective(
      lines, labels, name, cellSize, values, _argumentCompile);
}

MemoryDefinitionDirective<uint8_t>
RiscvDirectives::createMemoryDefinitionI8Directive(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    const std::vector<std::string>& values) {
  return createMemoryDefinitionDirective<uint8_t>(lines, labels, name, values);
}

MemoryDefinitionDirective<uint16_t>
RiscvDirectives::createMemoryDefinitionI16Directive(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    const std::vector<std::string>& values) {
  return createMemoryDefinitionDirective<uint16_t>(lines, labels, name, values);
}

MemoryDefinitionDirective<uint32_t>
RiscvDirectives::createMemoryDefinitionI32Directive(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    const std::vector<std::string>& values) {
  return createMemoryDefinitionDirective<uint32_t>(lines, labels, name, values);
}

MemoryDefinitionDirective<uint64_t>
RiscvDirectives::createMemoryDefinitionI64Directive(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    const std::vector<std::string>& values) {
  return createMemoryDefinitionDirective<uint64_t>(lines, labels, name, values);
}

MemoryReservationDirective RiscvDirectives::createMemoryReservationI8Directive(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    const std::vector<std::string>& values) {
  return createMemoryReservationDirective(
      lines, labels, name, sizeof(uint8_t), values);
}

MemoryReservationDirective RiscvDirectives::createMemoryReservationI16Directive(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    const std::vector<std::string>& values) {
  return createMemoryReservationDirective(
      lines, labels, name, sizeof(uint16_t), values);
}

MemoryReservationDirective RiscvDirectives::createMemoryReservationI32Directive(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    const std::vector<std::string>& values) {
  return createMemoryReservationDirective(
      lines, labels, name, sizeof(uint32_t), values);
}

MemoryReservationDirective RiscvDirectives::createMemoryReservationI64Directive(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    const std::vector<std::string>& values) {
  return createMemoryReservationDirective(
      lines, labels, name, sizeof(uint64_t), values);
}

ConstantDirective RiscvDirectives::createConstantDirective(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    const std::vector<std::string>& arguments) {
  return ConstantDirective(lines, labels, name, arguments);
}

MacroDirective RiscvDirectives::createMacroDirective(
    const LineInterval& lines,
    const std::vector<std::string>& labels,
    const std::string& name,
    const std::string& macroName,
    const std::vector<std::string>& macroParameters) {
  return MacroDirective(lines, labels, name, macroName, macroParameters);
}

MacroEndDirective
RiscvDirectives::createMacroEndDirective(const LineInterval& lines,
                                         const std::vector<std::string>& labels,
                                         const std::string& name) {
  return MacroEndDirective(lines, labels, name);
}

SectionDirective
RiscvDirectives::createSectionDirective(const LineInterval& lines,
                                        const std::vector<std::string>& labels,
                                        const std::string& name,
                                        const std::string& section) {
  return SectionDirective(lines, labels, name, section);
}
