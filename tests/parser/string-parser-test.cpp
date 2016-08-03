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

#include "parser/string-parser.hpp"
#include "gtest/gtest.h"

void doTest(const std::string& provided, const std::string& expected, bool succeed)
{
    std::vector<char> output;
    bool result = parseString(provided, output);
    ASSERT_EQ(result, succeed);
    if (succeed)
    {
        ASSERT_EQ(std::string(output.begin(), output.end()), expected);
    }
}

TEST(StringParser, emptyInput)
{
    doTest(R"()", "", false);
}

TEST(StringParser, wrongInput)
{
    doTest(R"(abcdefghijklmnopqrstuvwxyz1234567890!§$%&/()=?)", "", false);
}

TEST(StringParser, emptyString)
{
    doTest(R"("")", "", true);
}

TEST(StringParser, noSpecialsString)
{
    doTest(R"("abcdefghijklmnopqrstuvwxyz1234567890!§$%&/()=?")", "abcdefghijklmnopqrstuvwxyz1234567890!§$%&/()=?", true);
}

TEST(StringParser, genericErrors)
{
    doTest("\"\"\"", "", false);
    doTest("\"\\\"", "", false);
}

TEST(StringParser, simpleEscape)
{
    doTest(R"("\a")", "\a", true);
    doTest(R"("\b")", "\b", true);
    doTest(R"("\n")", "\n", true);
    doTest(R"("\r")", "\r", true);
    doTest(R"("\?")", "\?", true);
    doTest(R"("\"")", "\"", true);
    doTest(R"("\'")", "\'", true); //"' // <-- just a syntax highlighting fix.
    doTest(R"("\\")", "\\", true);
    doTest(R"("\ö")", "", false);
}

TEST(StringParser, octalString)
{
    doTest(R"("\7")", "\7", true);
    doTest(R"("\77")", "\77", true);
    doTest(R"("\377")", "\377", true);
    doTest(R"("\048")", "\048", true);
    doTest(R"("\047")", "\047", true);
    doTest(R"("\352")", "\352", true);
    doTest(R"("\352\352")", "\352\352", true);
    doTest(R"("#\352#\352#")", "#\352#\352#", true);
}

TEST(StringParser, hexXString)
{
    doTest(R"("\xf48e")", "\xf4", true);
    doTest(R"("\x048")", "\x04", true);
    doTest(R"("\x047")", "\x04", true);
    doTest(R"("\x352")", "\x35", true);
    doTest(R"("\xf4")", "\xf4", true);
    doTest(R"("\x14")", "\x14", true);
    doTest(R"("\xf4\x12")", "\xf4\x12", true);
}

TEST(StringParser, hexU4String)
{
    doTest(R"("\u0123")", "\u0123", true);
    doTest(R"("\uaffe")", "\uaffe", true);
    doTest(R"("\ubef")", "", false);
}

TEST(StringParser, hexU8String)
{
    doTest(R"("\U000dbeef")", "\U000dbeef", true);
    doTest(R"("\U00000123")", "\U00000123", true);
    doTest(R"("\U0000affe")", "\U0000affe", true);
    doTest(R"("\U0000bef")", "", false);
    doTest(R"("\Ubef")", "", false);
    doTest(R"("\Ubeef")", "", false);
}

TEST(StringParser, practicalString)
{
    doTest(R"("\r\n")", "\r\n", true);
    doTest(R"("\033[1m")", "\033[1m", true);
}
