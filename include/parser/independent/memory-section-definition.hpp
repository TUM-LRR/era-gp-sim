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

#ifndef ERAGPSIM_PARSER_MEMORY_SECTION_DEFINITION_HPP
#define ERAGPSIM_PARSER_MEMORY_SECTION_DEFINITION_HPP

#include <cstddef>
#include <string>

/**
 * \brief The definition of a section in memory.
 *
 * Basically, we got to define a name and the alignment properties so that
 * memory can be reserved.
 */
struct MemorySectionDefinition {
 public:
  using size_t = std::size_t;

  /**
   * \brief Creates a new memory section definition with the given parameters.
   * \param name The name of the section.
   * \param sectionAlignment The alignment of the section.
   * \param dataAlignment The alignment of the data.
   */
  MemorySectionDefinition(const std::string& name,
                          size_t sectionAlignment,
                          size_t dataAlignment);

  /**
   * \brief Creates a new memory section definition with the given parameters.
   * \param name The name of the section.
   * \param alignment The alignment of the section and the data.
   */
  explicit MemorySectionDefinition(const std::string& name,
                                   size_t alignment = 1);

  const std::string& name() const noexcept;

  size_t sectionAlignment() const noexcept;

  size_t dataAlignment() const noexcept;

 private:
  /**
   * \brief The name of the section.
   */
  std::string _name;

  /**
   * \brief The alignment of the whole section in memory.
   */
  size_t _sectionAlignment;

  /**
   * \brief The alignment of each new data block in memory.
   */
  size_t _dataAlignment;
};


#endif /* ERAGPSIM_PARSER_MEMORY_SECTION_DEFINITION_HPP */
