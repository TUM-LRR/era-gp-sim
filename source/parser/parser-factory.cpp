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

#include "parser/parser-factory.hpp"

#include "common/assert.hpp"
#include "parser/riscv-parser.hpp"

template <typename T>
static ParserPtr createParserInternal(const Architecture &arch,
                                      const MemoryAccess &memoryAccess) {
  return ParserPtr{new T{arch, memoryAccess}};
}

const std::unordered_map<std::string,
                         ParserPtr (*)(const Architecture &,
                                       const MemoryAccess &)>
    ParserFactory::mapping{{"riscv", createParserInternal<RiscvParser>}};

std::unique_ptr<Parser>
ParserFactory::createParser(const Architecture &arch,
                            const MemoryAccess &memoryAccess,
                            const std::string &name) {
  ParserPtr parser;

  // Try to find a parser matching name.
  auto element = mapping.find(name);

  assert::that(element != mapping.end());

  parser = (element->second)(arch, memoryAccess);

  return parser;
}
