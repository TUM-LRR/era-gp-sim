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

#include <string>
#include <vector>

#include "common/assert.hpp"
#include "core/memory.hpp"
#include "core/register-set.hpp"

Snapshot::Snapshot(const ArchitectureFormula& architectureFormula,
                   const Memory& memory,
                   const RegisterSet& registerSet)
: _snapshot() {
  _snapshot["architecture-name"] = architectureFormula.getArchitectureName();
  _snapshot["extensions"] = architectureFormula.getUnderlying();
  _snapshot["memory"] = memory.serializeJSON();
  _snapshot["registers"] = registerSet.serializeJSON();
}

Snapshot::Snapshot(Json json) : _snapshot(json) {
}

bool Snapshot::isValid() const {
  if (!_snapshot.count("architecture-name")) return false;
  if (!_snapshot.count("extensions")) return false;
  if (!_snapshot.count("memory")) return false;
  if (!_snapshot.count("registers")) return false;

  return true;
}

bool Snapshot::isValidProject() const {
  if (!isValid()) return false;
  if (!_snapshot.count("project-name")) return false;
  if (!_snapshot.count("parser-name")) return false;
  // if (!_snapshot.count("project-settings")) return false;
  if (!_snapshot.count("code")) return false;
  if (!_snapshot["memory"].count("memory_byteCount")) return false;

  return true;
}

ArchitectureFormula Snapshot::getArchitectureFormula() const {
  assert::that(isValid());
  std::string architectureName = _snapshot["architecture-name"];
  std::vector<std::string> extensions = _snapshot["extensions"];
  ArchitectureFormula formula(architectureName);
  for (const std::string& extension : extensions) {
    formula.addExtension(extension);
  }
  return formula;
}

Snapshot::Json Snapshot::getMemoryJson() const {
  assert::that(isValid());
  return _snapshot["memory"];
}

Snapshot::Json Snapshot::getRegisterJson() const {
  assert::that(isValid());
  return _snapshot["registers"];
}

std::string Snapshot::getCode() const {
  assert::that(isValidProject());
  return _snapshot["code"];
}

std::string Snapshot::getProjectName() const {
  assert::that(isValidProject());
  return _snapshot["project-name"];
}

std::string Snapshot::getParserName() const {
  assert::that(isValidProject());
  return _snapshot["parser-name"];
}

Memory::size_t Snapshot::getMemorySize() const {
  assert::that(isValidProject());
  return _snapshot["memory"].find("memory_byteCount")->get<Memory::size_t>();
}

void Snapshot::setCode(const std::string& code) {
  _snapshot["code"] = code;
}

void Snapshot::setProjectName(const std::string& projectName) {
  _snapshot["project-name"] = projectName;
}

void Snapshot::setParserName(const std::string& parserName) {
  _snapshot["parser-name"] = parserName;
}

Snapshot::Json Snapshot::getJson() const {
  return _snapshot;
}
