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

#ifndef ERAGPSIM_PARSER_INDEPENDENT_SECTION_TRACKER_HPP
#define ERAGPSIM_PARSER_INDEPENDENT_SECTION_TRACKER_HPP

#include <string>

/**
 * A small helper class to keep track of the current section while
 * parsing.
 */
class SectionTracker {
 public:
  /**
   * Creates a new section tracker which starts with the 'text' section.
   */
  SectionTracker();

  /**
   * Creates a new section tracker with the specified start section.
   */
  SectionTracker(const std::string& startSection);

  /**
   * Sets the section of this section tracker.
   * \param section The section to set.
   */
  void section(const std::string& section) noexcept;

  /**
   * Get the section of this section tracker.
   * \return The current section.
   */
  const std::string& section() const noexcept;

 private:
  /**
   * The currently tracked section.
   */
  std::string _section;
};

#endif /* ERAGPSIM_PARSER_INDEPENDENT_SECTION_TRACKER_HPP */
