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

#include "core/snapshot.hpp"
#include "common/assert.hpp"

Snapshot::Snapshot(const ArchitectureFormula& architectureFormula,
                   const Memory& memory,
                   const RegisterSet& registerSet)
: _snapshot() {
  _snapshot["architecture-name"] = architectureFormula.getArchitectureName();
  _snapshot["extensions"] = architectureFormula.getUnderlying();
  _snapshot["memory"] = memory.serializeJSON();
  _snapshot["registers"] = registerSet.serializeJSON();
}

Snapshot::Snapshot(Json json) _snapshot(json) {
}

bool Snapshot::isValid() {
  if (!(_snapshot.count("architecture-name") == 1)) return false;
  if (!(_snapshot.count("extensions") == 1)) return false;
  if (!(_snapshot.count("memory") == 1)) return false;
  if (!(_snapshot.count("registers") == 1)) return false;

  return true;
}

ArchitectureFormula Snapshot::getArchitectureFormula() {
  assert::that(isValid());
  std::string architectureName = _snapshot["architecture-name"];
  std::vector<std::string> extensions = _snapshot["extensions"];
  ArchitectureFormula formula(architectureName);
  for (const std::string& extension : extensions) {
    formula.addExtension(extension);
  }
  return formula;
}

Snapshot::Json Snapshot::getMemoryJson() {
  assert::that(isValid());
  return _snapshot["memory"];
}

Snapshot::Json Snapshot::getRegisterJson() {
  assert::that(isValid());
  return _snapshot["registers"];
}

Snapshot::Json Snapshot::getJson() {
  return _snapshot;
}
