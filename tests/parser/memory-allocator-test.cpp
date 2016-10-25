/* C++ Assembler Interpreter
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

#include "parser/memory-allocator.hpp"
#include "gtest/gtest.h"

TEST(MemoryAllocator, init) {
  MemoryAllocator allocator({MemorySectionDefinition("text", 4, 2),
                             MemorySectionDefinition("data", 16)});
}

TEST(MemoryAllocator, allocateAligned) {
  MemoryAllocator allocator({MemorySectionDefinition("text", 8),
                             MemorySectionDefinition("data", 16)});
  ASSERT_EQ(allocator["text"].allocateRelative(32).offset, 0);
  ASSERT_EQ(allocator["text"].allocateRelative(32).offset, 32);
  ASSERT_EQ(allocator["data"].allocateRelative(64).offset, 0);
  allocator.calculatePositions();
  ASSERT_EQ(allocator["text"].currentPosition(), 0);
  ASSERT_EQ(allocator["text"].currentSize(), 64);
  ASSERT_EQ(allocator["data"].currentPosition(), 64);
  ASSERT_EQ(allocator["data"].currentSize(), 64);
}

TEST(MemoryAllocator, allocateUnaligned) {
  MemoryAllocator allocator({MemorySectionDefinition("text", 32),
                             MemorySectionDefinition("data", 32)});
  ASSERT_EQ(allocator["text"].allocateRelative(13).offset, 0);
  ASSERT_EQ(allocator["text"].allocateRelative(129).offset, 32);
  ASSERT_EQ(allocator["data"].allocateRelative(23).offset, 0);
  allocator.calculatePositions();
  ASSERT_EQ(allocator["text"].currentPosition(), 0);
  ASSERT_EQ(allocator["text"].currentSize(), 161);
  ASSERT_EQ(allocator["data"].currentPosition(), 192);
  ASSERT_EQ(allocator["data"].currentSize(), 23);
}

TEST(MemoryAllocator, allocateUnalignedSection) {
  MemoryAllocator allocator({MemorySectionDefinition("text", 256, 32),
                             MemorySectionDefinition("data", 256, 32)});
  ASSERT_EQ(allocator["text"].allocateRelative(13).offset, 0);
  ASSERT_EQ(allocator["text"].allocateRelative(129).offset, 32);
  ASSERT_EQ(allocator["data"].allocateRelative(23).offset, 0);
  allocator.calculatePositions();
  ASSERT_EQ(allocator["text"].currentPosition(), 0);
  ASSERT_EQ(allocator["text"].currentSize(), 161);
  ASSERT_EQ(allocator["data"].currentPosition(), 256);
  ASSERT_EQ(allocator["data"].currentSize(), 23);
}
