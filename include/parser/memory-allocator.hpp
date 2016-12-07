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

#ifndef ERAGPSIM_PARSER_MEMORY_ALLOCATOR_HPP
#define ERAGPSIM_PARSER_MEMORY_ALLOCATOR_HPP

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/assert.hpp"
#include "common/utility.hpp"

#include "parser/memory-section-definition.hpp"
#include "parser/relative-memory-position.hpp"

/**
 * \brief Manages the allocation of memory without ever making use of it.
 *
 * Yes, this class reserves memory but never really sets something to it. It
 * also does not check if there is still something free, in hope that this will
 * be thrown later on.
 */
class MemoryAllocator {
 public:
  using size_t = std::size_t;

  /**
   * \brief A memory section which can allocate.
   */
  class MemorySection {
   public:
    friend MemoryAllocator;

    /**
     * \brief Create a new memory section out of a definition of it.
     * \param definition The definition of the memory section.
     */
    MemorySection(const MemorySectionDefinition& definition);

    /**
     * \brief Frees the whole memory section (all previous reservations are
     * removed).
     */
    void clear();

    /**
     * \brief Reserves the given amout of memory.
     * \param size The given amount of memory.
     * \return The position where the memory starts in the given section.
     */
    RelativeMemoryPosition allocateRelative(size_t size);

    /**
     * \brief The current size of the section.
     * \return The current size of the section.
     */
    size_t currentSize() const;

    /**
     * \brief The absolute position of this section in memory.
     * \return The absolute position of this section in memory.
     */
    size_t currentPosition() const;

    /**
     * \brief Converts the given relative address into an absolute one.
     * \param relative The relative position.
     * \return The absolute position.
     */
    size_t absoluteAddress(const RelativeMemoryPosition& relative) const;

    /**
     * \brief Converts the given relative address into an absolute one.
     * \param relative The relative position.
     * \return The absolute position.
     */
    size_t absoluteAddress(size_t relative) const;

   private:
    /**
     * \brief The definition of this memory section.
     */
    MemorySectionDefinition _definition;

    /**
     * \brief The current size of the memory section.
     */
    size_t _currentSize;

    /**
     * \brief The current absolute position of this memory section.
     */
    size_t _currentPosition;
  };

  /**
   * \brief Creates a new memory allocator with the given memory section
   * definitions.
   * \param sectionDefinitions The definition vector of the memory sections.
   */
  MemoryAllocator(
      const std::vector<MemorySectionDefinition>& sectionDefinitions);

  /**
   * \brief Clears all sections (i.e. un-reserves all memory).
   */
  void clear();

  /**
   * \brief Calculates the absolute positions of the memory sections.
   * \return The complete occupied size of the memory.
   */
  size_t calculatePositions();

  /**
   * \brief Accessor for the sections by name.
   * \param name The name of the section to be retrieved.
   * \return A reference on the memory section.
   */
  MemorySection& operator[](const std::string& name);

  /**
   * \brief Accessor for the sections by index.
   * \param index The index of the section to be retrieved.
   * \return A reference on the memory section.
   */
  MemorySection& operator[](size_t index);

  /**
   * \brief Constant accessor for the sections by name.
   * \param name The name of the section to be retrieved.
   * \return A reference on the memory section.
   */
  const MemorySection& at(const std::string& name) const;

  /**
   * \brief Constant accessor for the sections by index.
   * \param index The index of the section to be retrieved.
   * \return A reference on the memory section.
   */
  const MemorySection& at(size_t index) const;

  /**
   * \brief Transforms the given relative position into an absolute one.
   * \param relative The given relative position.
   * \return The calculated absolute position.
   */
  size_t absolutePosition(const RelativeMemoryPosition& relative) const;

  /**
   * \brief Checks if the given string the name of a section.
   * \param name The name to check.
   * \return True if and only if the name if the name of a memory section.
   */
  bool has(const std::string& name) const noexcept;

 private:
  /**
   * \brief The vector of stored memory section objects.
   */
  std::vector<MemorySection> _sections;

  /**
   * \brief An index mapping for accessing the sections by name.
   */
  std::unordered_map<std::string, size_t> _nameToSection;
};

#endif /* ERAGPSIM_PARSER_MEMORY_ALLOCATOR_HPP */
