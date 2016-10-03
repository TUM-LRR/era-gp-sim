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

#include <gtest/gtest.h>
#include <string>

#include "common/assert.hpp"

TEST(AssertionTests, WorksForTruthyExpressions) {
  EXPECT_NO_THROW(assert::that(5 == 5));
  EXPECT_NO_THROW(assert::that(std::string("foo") != std::string("bar")));
}

TEST(AssertionTests, WorksForFalsyExpressions) {
  EXPECT_THROW(assert::that(false), assert::AssertionError);
  EXPECT_THROW(assert::that(5 == 7), assert::AssertionError);
  EXPECT_THROW(assert::that(std::string("foo") == std::string("bar")),
               assert::AssertionError);
}

// Mad scientist time
// This will only include what would have been included if the DEBUG symbol
// had not been set before including the header initially (as if via a
// command-line flag)
#undef ERAGPSIM_COMMON_ASSERT_HPP
#undef DEBUG
#undef that
#undef gtest
#include "common/assert.hpp"


TEST(AssertionTests, DoesNotThrowForFalsyExpressionsWhenDEBUGNotDefined) {
  EXPECT_NO_THROW(assert::that(false));
  EXPECT_NO_THROW(assert::that(5 == 7));
  EXPECT_NO_THROW(assert::that(std::string("foo") == std::string("bar")));
}
