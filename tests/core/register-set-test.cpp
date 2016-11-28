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

#include <random>
#include <sstream>
#include <string>
#include <vector>

// Gtest has to be included before memory-value.
// clang-format off
#include "gtest/gtest.h"
#include "core/memory-value.hpp"
#include "core/register-set.hpp"
#include "core/conversions.hpp"
// clang-format on

namespace {
constexpr std::size_t scale = 10;
}

TEST(registerSet, createRw) {
  constexpr std::size_t b = 1024; // size
  constexpr std::size_t t = scale;// testAmount
  RegisterSet instance{};
  std::vector<std::string> identifierList{""};
  std::uniform_int_distribution<std::uint16_t> dist{0, 255};
  std::mt19937 rand(0);// I need new numbers, I'm kinda really out of ideas
  for (std::size_t i = 1; i < b; ++i) {
    std::stringstream strm{};
    strm << "register_";
    strm << i;
    identifierList.push_back(strm.str());
    instance.createRegister(strm.str(), i);
  }
  for (std::size_t i = 1; i < b; ++i) {
    std::size_t byteSize{(i + 7) / 8};
    for (std::size_t j = 0; j < t; ++j) {
      std::vector<std::uint8_t> initializer{};
      for (std::size_t l = 0; l < byteSize; ++l) {
        initializer.push_back(static_cast<std::uint8_t>(dist(rand)));
      }
      MemoryValue instance0{initializer, i};
      instance.put(identifierList[i], instance0);
      ASSERT_EQ(instance0, instance.get(identifierList[i]));
    }
  }
}

TEST(registerSet, aliasRwRand) {
  constexpr std::size_t b = 1024; // size
  constexpr std::size_t t = scale;// testAmount
  const std::string parent = "lilith";
  RegisterSet instance{};
  std::uniform_int_distribution<std::uint16_t> dist{0, 255};
  std::mt19937 rand(0);// I need new numbers, I'm kinda really out of ideas
  instance.createRegister(parent, b * 2);
  for (std::size_t i = 1; i < b; ++i) {
    std::size_t byteSize{(i + 7) / 8};
    std::uniform_int_distribution<std::size_t> dist0{0, (2 * b) - i - 1};
    for (std::size_t j = 0; j < t; ++j) {
      std::vector<std::uint8_t> initializer{};
      for (std::size_t l = 0; l < byteSize; ++l) {
        initializer.push_back(static_cast<std::uint8_t>(dist(rand)));
      }
      MemoryValue instance0{initializer, i};
      MemoryValue parentOldValue{instance.get(parent)};
      std::size_t address{dist0(rand)};
      std::stringstream strm{};
      strm << "register_";
      strm << i;
      strm << '_';
      strm << j;
      instance.aliasRegister(strm.str(), parent, address, address + i);
      MemoryValue mid = instance.set(strm.str(), instance0);
      ASSERT_EQ(instance0, instance.get(strm.str()));
      MemoryValue parentNewValue{instance.get(parent)};
      if (address > 0) {
        ASSERT_EQ(parentOldValue.subSet(0, address),
                  parentNewValue.subSet(0, address));
      }
      ASSERT_EQ(parentOldValue.subSet(address, address + i), mid);
      if (address + i < (2 * b) - 1) {
        ASSERT_EQ(parentOldValue.subSet(address + i, 2 * b),
                  parentNewValue.subSet(address + i, 2 * b));
      }
    }
  }
}

TEST(registerSet, aliasRwTransitive) {
  constexpr std::size_t b = 1024; // size
  constexpr std::size_t t = scale;// testAmount
  const std::string parent = "lilith";
  RegisterSet instance{};
  std::uniform_int_distribution<std::uint16_t> dist{0, 255};
  std::mt19937 rand(0);// I need new numbers, I'm kinda really out of ideas
  instance.createRegister(parent, b + 1);
  for (std::size_t j = 0; j < t; ++j) {
    std::string prev{parent};
    for (std::size_t i = b; i > 0; --i) {
      std::size_t byteSize{(i + 7) / 8};
      std::vector<std::uint8_t> initializer{};
      for (std::size_t l = 0; l < byteSize; ++l) {
        initializer.push_back(static_cast<std::uint8_t>(dist(rand)));
      }
      MemoryValue instance0{initializer, i};
      std::stringstream strm{};
      strm << "register_";
      strm << j;
      strm << '_';
      strm << i;
      std::size_t address{static_cast<std::size_t>(dist(rand) % 2)};
      instance.aliasRegister(strm.str(), prev, address, i + address);
      instance.put(strm.str(), instance0);
      ASSERT_EQ(instance0, instance.get(strm.str()));
      prev = strm.str();
    }
  }
}

TEST(registerSet, updateTest) {
  const std::string parentA = "I_AM_YOUR_FATHER";
  const std::string parentB = "I_AM_SAD";
  const std::string childA = "I_AM_THE_CHOSEN_ONE";
  const std::string childB = "I_AM_TRUTH_AND_ETERNITY";
  const std::string childC = "I_AM_JUSTICE_FOR_THEM_ALL";
  const std::string lambdaTest = "I_AM_THE_IRRELEVANT";
  RegisterSet instance{};
  std::set<std::string> updateSet{};
  auto lambda = [&](const std::string& regName) mutable {
    updateSet.insert(regName);
  };
  lambda(lambdaTest);
  ASSERT_NE(updateSet.find(lambdaTest), updateSet.end());
  instance.setCallback(lambda);
  instance.createRegister(parentA, 64);
  instance.createRegister(parentB, 64);
  ASSERT_NE(updateSet.find(parentA), updateSet.end());
  instance.aliasRegister(childA, parentA);
  instance.aliasRegister(childB, parentA);
  instance.aliasRegister(childC, parentA, 0, true);// silent
  // alias does not callback
  ASSERT_EQ(updateSet.find(childA), updateSet.end());
  updateSet.clear();
  ASSERT_EQ(updateSet.find(parentA), updateSet.end());
  instance.put(parentA, MemoryValue(64));
  ASSERT_NE(updateSet.find(parentA), updateSet.end());
  ASSERT_NE(updateSet.find(childA), updateSet.end());
  ASSERT_NE(updateSet.find(childB), updateSet.end());
  ASSERT_EQ(updateSet.find(childC), updateSet.end());
  updateSet.clear();
  instance.put(childA, MemoryValue(64));
  ASSERT_NE(updateSet.find(parentA), updateSet.end());
  ASSERT_NE(updateSet.find(childA), updateSet.end());
  ASSERT_NE(updateSet.find(childB), updateSet.end());
  ASSERT_EQ(updateSet.find(childC), updateSet.end());
  updateSet.clear();
  instance.put(childB, MemoryValue(64));
  ASSERT_NE(updateSet.find(parentA), updateSet.end());
  ASSERT_NE(updateSet.find(childA), updateSet.end());
  ASSERT_NE(updateSet.find(childB), updateSet.end());
  ASSERT_EQ(updateSet.find(childC), updateSet.end());
  updateSet.clear();
  instance.put(childC, MemoryValue(64));
  ASSERT_NE(updateSet.find(parentA), updateSet.end());
  ASSERT_NE(updateSet.find(childA), updateSet.end());
  ASSERT_NE(updateSet.find(childB), updateSet.end());
  ASSERT_EQ(updateSet.find(childC), updateSet.end());
  updateSet.clear();
  instance.put(parentB, MemoryValue(64));
  ASSERT_EQ(updateSet.find(parentA), updateSet.end());
  ASSERT_NE(updateSet.find(parentB), updateSet.end());
  ASSERT_EQ(updateSet.find(childA), updateSet.end());
  ASSERT_EQ(updateSet.find(childB), updateSet.end());
  ASSERT_EQ(updateSet.find(childC), updateSet.end());
}

TEST(registerSet, serialization) {
  std::vector<std::string> reg{};
  reg.push_back("I_NEED_LOTSA_UNIQUE_NAMES");
  reg.push_back("I_AM_KINDA_NOT_VERY_CREATIVE_TODAY");
  reg.push_back("SO_MANY_POSSIBILITIES");
  reg.push_back("YAY_FOR_REGISTER_NAMES");
  reg.push_back("I_HAVE_GREAT_IDEAS_FOR_OTHER_THINGS_THO");
  reg.push_back("WOULD_IT_BE_LEGAL_TO_USE_A_SONGTEXT_AS_CONSTANT?");
  reg.push_back("PROBABLY_NOT");
  reg.push_back("THAT'S_TOO_BAD");
  reg.push_back("I'D_LOVE_TO");
  reg.push_back("LET_IT_GO");
  reg.push_back("I_AM_HUNGRY");
  reg.push_back("AND_TIRED");
  reg.push_back("I_WANNA_GO_HOME_NOW");
  reg.push_back("#RegisterBadLuck");
  reg.push_back("THE_VERY_LAST_REGISTER");
  reg.push_back("NOT_A_REGISTER");
  std::uniform_int_distribution<std::uint32_t> dist{};
  std::mt19937 rand(0);// I need new numbers, I'm kinda really out of ideas
  RegisterSet instance0{};
  for (const auto& r : reg) {
    instance0.createRegister(r, 32);
  }
  RegisterSet instance1{instance0};
  for (const auto& r : reg) {
    instance0.put(
        r,
        conversions::convert(
            dist(rand), conversions::standardConversions::nonsigned, 32));
  }
  auto json = instance0.serializeJSON();
  instance1.deserializeJSON(json);
  ASSERT_EQ(instance0, instance1);
}
