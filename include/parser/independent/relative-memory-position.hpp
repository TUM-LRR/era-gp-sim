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

#ifndef ERAGPSIM_PARSER_RELATIVE_MEMORY_POSITION_HPP
#define ERAGPSIM_PARSER_RELATIVE_MEMORY_POSITION_HPP

#include <cstddef>
#include <string>

/**
 * \brief A relative memory position in one specific section.
 */
struct RelativeMemoryPosition {
 public:
  using size_t = std::size_t;

  /**
   * \brief Creates a new relative memory position with the given parameters.
   * \param section The given section where the data is stored.
   * \param offset The given offset from the beginning of the section.
   */
  RelativeMemoryPosition(const std::string& section, size_t offset);

  /**
   * \brief Creates a new empty relative memory position.
   */
  RelativeMemoryPosition();

  /**
   * \brief Checks if the relative memory position is not in an empty section.
   * \return True, if section != "".
   */
  bool valid() const;

  const std::string& section() const noexcept;

  size_t offset() const noexcept;

 private:
  /**
   * \brief The section which contains this memory piece.
   */
  std::string _section;

  /**
   * \brief The offset to the beginning of this section.
   */
  size_t _offset;
};

#endif /* ERAGPSIM_PARSER_RELATIVE_MEMORY_POSITION_HPP */
