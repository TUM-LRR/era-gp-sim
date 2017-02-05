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

#include <string>

#include "arch/common/architecture-formula.hpp"
#include "core/memory.hpp"
#include "third-party/json/json.hpp"

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
  explicit Snapshot(Json json);

  /**
   * \return true if the snapshot is valid, false if not.
   * This method only checks the structure of the snapshot, the content of the
   * components could still be invalid.
   */
  bool isValid() const;

  /**
   * Checks if this snapshot object is a valid snapshot and a valid project. A
   * valid project additionally contains text of the editor and settings.
   * Like the `isValid()` method, this only checks the structure of the json,
   * not the content
   *
   * \return True if the snapshot is a project, false otherwise.
   */
  bool isValidProject() const;

  /**
   * \return A json object which can be used to deserialize the memory.
   */
  Json getMemoryJson() const;

  /**
   * \return A json object which can be used to deserialize the
   * registers.
   */
  Json getRegisterJson() const;

  /**
   * \return The architecture formula of this snapshot.
   */
  ArchitectureFormula getArchitectureFormula() const;

  /**
   * \return The code in this snapshots. Assure that the snapshot is a valid
   * project snapshot, first.
   */
  std::string getCode() const;

  /**
   * \return The Project name.
   */
  std::string getProjectName() const;

  /**
   * \return The Parser name.
   */
  std::string getParserName() const;

  /**
   * \return The memory size.
   */
  Memory::size_t getMemorySize() const;

  /**
   * \return The project settings.
   */
  Json getProjectSettings() const;

  /**
   * Set the code property of this snapshot.
   *
   * \param code The code (text).
   */
  void setCode(const std::string& code);

  /**
   * Set the project-name property of this snapshot.
   *
   * \param projectName The project name.
   */
  void setProjectName(const std::string& projectName);

  /**
   * Set the parser-name property of this snapshot.
   *
   * \param parserName The parser name.
   */
  void setParserName(const std::string& parserName);

  /**
   * Sets the project-settings property.
   *
   * \param json The project settings json-object.
   */
  void setProjectSettings(const Json& json);

  /**
   * \return The underlying json object.
   */
  Json getJson() const;

 private:
  Json _snapshot;
};

#endif /* ERAGPSIM_CORE_SNAPSHOT_HPP */
