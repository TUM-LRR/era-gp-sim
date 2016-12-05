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

#include <string>
#include <unordered_map>
#include <vector>
#include "common/assert.hpp"
#include "common/utility.hpp"

/**
 * \brief A relative memory position in one specific section.
 */
struct RelativeMemoryPosition {
  /**
   * \brief The section which contains this memory piece.
   */
  std::string section;

  /**
   * \brief The offset to the beginning of this section.
   */
  std::size_t offset;

  /**
   * \brief Creates a new relative memory position with the given parameters.
   * \param section The given section where the data is stored.
   * \param offset The given offset from the beginning of the section.
   */
  RelativeMemoryPosition(const std::string& section, std::size_t offset);

  /**
   * \brief Creates a new empty relative memory position.
   */
  RelativeMemoryPosition();

  /**
   * \brief Checks if the relative memory position is not in an empty section.
   * \return True, if section != "".
   */
  bool valid();
};

/**
 * \brief The definition of a section in memory.
 *
 * Basically, we got to define a name and the alignment properties so that
 * memory can be reserved.
 */
struct MemorySectionDefinition {
  /**
   * \brief The name of the section.
   */
  std::string name;

  /**
   * \brief The alignment of the whole section in memory.
   */
  std::size_t sectionAlignment;

  /**
   * \brief The alignment of each new data block in memory.
   */
  std::size_t dataAlignment;

  /**
   * \brief Creates a new memory section definition with the given parameters.
   * \param name The name of the section.
   * \param sectionAlignment The alignment of the section.
   * \param dataAlignment The alignment of the data.
   */
  MemorySectionDefinition(const std::string& name,
                          std::size_t sectionAlignment,
                          std::size_t dataAlignment);

  /**
   * \brief Creates a new memory section definition with the given parameters.
   * \param name The name of the section.
   * \param alignment The alignment of the section and the data.
   */
  MemorySectionDefinition(const std::string& name, std::size_t alignment);

  /**
   * \brief Creates a new memory section definition with the given name.
   * \param name The name of the section.
   */
  MemorySectionDefinition(const std::string& name);
};

/**
 * \brief Manages the allocation of memory without ever making use of it.
 *
 * Yes, this class reserves memory but never really sets something to it. It
 * also does not check if there is still something free, in hope that this will
 * be thrown later on.
 */
class MemoryAllocator {
 public:
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
    RelativeMemoryPosition allocateRelative(std::size_t size);

    /**
     * \brief The current size of the section.
     * \return The current size of the section.
     */
    std::size_t currentSize() const;

    /**
     * \brief The absolute position of this section in memory.
     * \return The absolute position of this section in memory.
     */
    std::size_t currentPosition() const;

    /**
     * \brief Converts the given relative address into an absolute one.
     * \param relative The relative position.
     * \return The absolute position.
     */
    std::size_t absoluteAddress(const RelativeMemoryPosition& relative) const;

    /**
     * \brief Converts the given relative address into an absolute one.
     * \param relative The relative position.
     * \return The absolute position.
     */
    std::size_t absoluteAddress(std::size_t relative) const;

   private:
    /**
     * \brief The definition of this memory section.
     */
    MemorySectionDefinition _definition;

    /**
     * \brief The current size of the memory section.
     */
    std::size_t _currentSize;

    /**
     * \brief The current absolute position of this memory section.
     */
    std::size_t _currentPosition;
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
  std::size_t calculatePositions();

  /**
   * \brief Accessor for the sections by name.
   * \param name The name of the section to be retrieved.
   * \return A reference on the memory section.
   */
  MemorySection& operator[](std::string name);

  /**
   * \brief Accessor for the sections by index.
   * \param index The index of the section to be retrieved.
   * \return A reference on the memory section.
   */
  MemorySection& operator[](std::size_t index);

  /**
   * \brief Constant accessor for the sections by name.
   * \param name The name of the section to be retrieved.
   * \return A reference on the memory section.
   */
  const MemorySection& at(std::string name) const;

  /**
   * \brief Constant accessor for the sections by index.
   * \param index The index of the section to be retrieved.
   * \return A reference on the memory section.
   */
  const MemorySection& at(std::size_t index) const;

  /**
   * \brief Transforms the given relative position into an absolute one.
   * \param relative The given relative position.
   * \return The calculated absolute position.
   */
  std::size_t absolutePosition(const RelativeMemoryPosition& relative) const;

 private:
  /**
   * \brief The vector of stored memory section objects.
   */
  std::vector<MemorySection> _sections;

  /**
   * \brief An index mapping for accessing the sections by name.
   */
  std::unordered_map<std::string, std::size_t> _nameToSection;
};

#endif /* ERAGPSIM_PARSER_MEMORY_ALLOCATOR_HPP */
