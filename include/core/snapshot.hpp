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

#ifndef ERAGPSIM_CORE_SNAPSHOT_HPP
#define ERAGPSIM_CORE_SNAPSHOT_HPP

#include "third-party/json/json.hpp"

class ArchitectureFormula;
class Memory;
class RegisterSet;

class Snapshot {
 public:
  using Json = nlohmann::json;

  /**
   * Creates a new snapshot of a project.
   *
   * \param architectureFormula The architecture formula of the project.
   * \param memory The memory of the project.
   * \param registerSet The registers of the project.
   */
  Snapshot(const ArchitectureFormula& architectureFormula,
           const Memory& memory,
           const RegisterSet& registerSet);

  /**
   * Creates a snapshot from a json object.
   *
   * \param json The json object to load.
   */
  Snapshot(Json json);

  /**
   * \return Returns true if the snapshot is valid, false if not.
   * This method only checks the structure of the snapshot, the content of the
   * components could still be invalid.
   */
  bool isValid();

  /**
   * \return returns a json object which can be used to deserialize the memory.
   */
  Json getMemoryJson();

  /**
   * \return Returns a json object which can be used to deserialize the
   * registers.
   */
  Json getRegisterJson();

  /**
   * \return Returns the architecture formula of this snapshot.
   */
  ArchitectureFormula getArchitectureFormula();

  /**
   * \return Returns the underlying json object.
   */
  Json getJson();

 private:
  Json _snapshot;
};

#endif /* ERAGPSIM_CORE_SNAPSHOT_HPP */
