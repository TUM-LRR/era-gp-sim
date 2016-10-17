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
#include "common/utility.hpp"
#include "common/assert.hpp"

struct RelativeMemoryPosition {
    std::string section;
    std::size_t offset;

    RelativeMemoryPosition(const std::string& section, std::size_t offset);

    RelativeMemoryPosition();
};

struct MemorySectionDefinition {
    std::string name;
    std::size_t sectionAlignment;
    std::size_t dataAlignment;

    MemorySectionDefinition(const std::string& name, std::size_t sectionAlignment, std::size_t dataAlignment);

    MemorySectionDefinition(const std::string& name, std::size_t alignment);

    MemorySectionDefinition(const std::string& name);
};

class MemoryAllocator {
public:
    class MemorySection {
    public:
        friend MemoryAllocator;

        MemorySection(const MemorySectionDefinition& definition);

        void clear();

        std::size_t allocateRelative(std::size_t size);

        std::size_t currentSize() const;

        std::size_t currentPosition() const;

        std::size_t absoluteAddress(std::size_t relative) const;

    private:
        MemorySectionDefinition _definition;
        std::size_t _currentSize;
        std::size_t _currentPosition;
    };

    MemoryAllocator(const std::vector<MemorySectionDefinition>& sectionDefinitions);

    void clear();

    std::size_t calculatePositions();

    MemorySection& operator[](std::string name);

    MemorySection& operator[](std::size_t index);

    std::size_t absolutePosition(const RelativeMemoryPosition& relative) const;
private:
    std::vector<MemorySection> _sections;
    std::unordered_map<std::string, std::size_t> _nameToSection;
};

#endif /* ERAGPSIM_PARSER_MEMORY_ALLOCATOR_HPP */
