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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#include "gtest/gtest.h"

#include "arch/common/unit-information.hpp"

struct UnitInformationTestFixture : public ::testing::Test {
  using SortedResult = UnitInformation::SortedResult;

  UnitInformationTestFixture() {
    using Type = RegisterInformation::Type;
    // fill _unitInformation with some values
    addRegister("a0", 4);
    addRegister("a1", 3);
    addRegister("c3", 7);
    addRegister("b2", 1);
    addRegister("x45", 5);

    addRegister("flag1", 6, Type::FLAG);
    addRegister("pc", 0, Type::PROGRAM_COUNTER);
    addRegister("link2", 100, Type::LINK);
  }

 protected:
  void addRegister(
      const std::string& name, RegisterInformation::id_t id,
      RegisterInformation::Type type = RegisterInformation::Type::INTEGER) {
    RegisterInformation registerInfo{};
    registerInfo.name(name).id(id).type(type);
    _unitInformation.addRegister(registerInfo);
  }

  template <typename ExpectedRange>
  void compareResult(ExpectedRange expected, SortedResult result) const {
    ASSERT_EQ(expected.size(), result.size());
    for (auto i = 0; i < result.size(); ++i) {
      ASSERT_EQ(expected[i], result[i]->getName());
    }
  }

  UnitInformation _unitInformation;
};

TEST_F(UnitInformationTestFixture, getRegistersSortedAlphabetic) {
  std::vector<std::string> expected = {"a0", "a1", "b2", "c3", "x45"};
  compareResult(expected, _unitInformation.getRegisterSorted(
                              UnitInformation::AlphabeticOrder{}));
}

TEST_F(UnitInformationTestFixture, getRegistersSortedId) {
  std::vector<std::string> expected = {"b2", "a1", "a0", "x45", "c3"};
  compareResult(expected,
                _unitInformation.getRegisterSorted(UnitInformation::IdOrder{}));
}

TEST_F(UnitInformationTestFixture, getSpecialRegistersSortedAlphabetic) {
  std::vector<std::string> expected = {"flag1", "link2", "pc"};
  compareResult(expected, _unitInformation.getSpecialRegisterSorted(
                              UnitInformation::AlphabeticOrder{}));
}

TEST_F(UnitInformationTestFixture, getSpecialRegistersSortedId) {
  std::vector<std::string> expected = {"pc", "flag1", "link2"};
  compareResult(expected, _unitInformation.getSpecialRegisterSorted(
                              UnitInformation::IdOrder{}));
}

TEST_F(UnitInformationTestFixture, getAllRegistersSortedAlphabetic) {
  std::vector<std::string> expected = {"a0",    "a1",    "b2", "c3",
                                       "flag1", "link2", "pc", "x45"};
  auto comp = UnitInformation::AlphabeticOrder{};
  compareResult(expected, _unitInformation.getAllRegisterSorted(comp));
}

TEST_F(UnitInformationTestFixture, getAllRegistersSortedId) {
  std::vector<std::string> expected = {"pc",  "b2",    "a1", "a0",
                                       "x45", "flag1", "c3", "link2"};
  auto comp = UnitInformation::IdOrder{};
  compareResult(expected, _unitInformation.getAllRegisterSorted(comp));
}

TEST_F(UnitInformationTestFixture, getAllRegistersSortedType) {
  using Type = RegisterInformation::Type;
  std::vector<std::string> expected = {"flag1", "pc", "a0",  "a1",
                                       "b2",    "c3", "x45", "link2"};
  auto comp = UnitInformation::TypeOrder(
      {Type::FLAG, Type::PROGRAM_COUNTER, Type::VECTOR, Type::INTEGER});
  compareResult(expected, _unitInformation.getAllRegisterSorted(comp));
}
