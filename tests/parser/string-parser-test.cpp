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

#include "parser/independent/string-parser.hpp"
#include <iostream>
#include "common/translateable.hpp"
#include "gtest/gtest.h"
#include "parser/common/compile-error-list.hpp"
#include "parser/common/compile-error-list.hpp"
#include "parser/independent/positioned-string.hpp"

template <typename CharTypeIn, typename CharTypeOut>
void doTestInternal(const std::basic_string<CharTypeIn>& provided,
                    const std::basic_string<CharTypeOut>& expected,
                    bool succeed) {
  CompileErrorList errors;
  std::vector<CharTypeOut> output;
  PositionedBasicString<CharTypeIn> input(provided);
  bool result = StringParser::parseString(input, errors, output);
  if (succeed) {
    for (const auto& error : errors.errors()) {
      std::cout << error.message().getBaseString() << std::endl;
    }
    ASSERT_EQ(expected,
              std::basic_string<CharTypeOut>(output.begin(), output.end()));
    ASSERT_EQ(0, errors.size());
  }
  ASSERT_EQ(succeed, result);
}

void doTest(const std::string& provided,
            const std::string& expected,
            bool succeed) {
  doTestInternal(provided, expected, succeed);
}

void doTest(const std::string& provided,
            const std::u16string& expected,
            bool succeed) {
  doTestInternal(provided, expected, succeed);
}

void doTest(const std::string& provided,
            const std::u32string& expected,
            bool succeed) {
  doTestInternal(provided, expected, succeed);
}

void doTest(const std::u16string& provided,
            const std::string& expected,
            bool succeed) {
  doTestInternal(provided, expected, succeed);
}

void doTest(const std::u16string& provided,
            const std::u16string& expected,
            bool succeed) {
  doTestInternal(provided, expected, succeed);
}

void doTest(const std::u16string& provided,
            const std::u32string& expected,
            bool succeed) {
  doTestInternal(provided, expected, succeed);
}

void doTest(const std::u32string& provided,
            const std::string& expected,
            bool succeed) {
  doTestInternal(provided, expected, succeed);
}

void doTest(const std::u32string& provided,
            const std::u16string& expected,
            bool succeed) {
  doTestInternal(provided, expected, succeed);
}

void doTest(const std::u32string& provided,
            const std::u32string& expected,
            bool succeed) {
  doTestInternal(provided, expected, succeed);
}

TEST(StringParser, emptyInput) {
  doTest(u8R"()", u8"", false);
}

TEST(StringParser, wrongInput) {
  doTest(u8R"(abcdefghijklmnopqrstuvwxyz1234567890!§$%&/()=?)", u8"", false);
}

TEST(StringParser, emptyString) {
  doTest(u8R"("")", u8"", true);
}

TEST(StringParser, noSpecialsString) {
  doTest(u8R"("abcdefghijklmnopqrstuvwxyz1234567890!§$%&/()=?")",
         u8"abcdefghijklmnopqrstuvwxyz1234567890!§$%&/()=?",
         true);
}

TEST(StringParser, genericErrors) {
  doTest("\"\"\"", u8"", false);
  doTest("\"\\\"", u8"", false);
}

TEST(StringParser, simpleEscape) {
  doTest(u8R"("\a")", u8"\a", true);
  doTest(u8R"("\b")", u8"\b", true);
  doTest(u8R"("\n")", u8"\n", true);
  doTest(u8R"("\r")", u8"\r", true);
  doTest(u8R"("\?")", u8"\?", true);
  doTest(u8R"("\"")", u8"\"", true);
  doTest(u8R"("\'")", u8"\'", true);
  doTest(u8R"("\\")", u8"\\", true);
  doTest(u8R"("\ö")", u8"", false);
}

TEST(StringParser, octalString) {
  doTest(u8R"("\7")", u8"\7", true);
  doTest(u8R"("\77")", u8"\77", true);
  doTest(u8R"("\377")", u8"\377", true);
  doTest(u8R"("\048")", u8"\048", true);
  doTest(u8R"("\047")", u8"\047", true);
  doTest(u8R"("\352")", u8"\352", true);
  doTest(u8R"("\352\352")", u8"\352\352", true);
  doTest(u8R"("#\352#\352#")", u8"#\352#\352#", true);
}

TEST(StringParser, hexXString) {
  doTest(u8R"("\xf4")", u8"\xf4", true);
  doTest(u8R"("\x14")", u8"\x14", true);
  doTest(u8R"("\xf4\x12")", u8"\xf4\x12", true);
}

TEST(StringParser, hexU4String) {
  doTest(u8R"("\u0123")", u8"\u0123", true);
  doTest(u8R"("\uaffe")", u8"\uaffe", true);
  doTest(u8R"("\ubef")", u8"", false);
}

TEST(StringParser, hexU8String) {
  doTest(u8R"("\U000dbeef")", u8"\U000dbeef", true);
  doTest(u8R"("\U00000123")", u8"\U00000123", true);
  doTest(u8R"("\U0000affe")", u8"\U0000affe", true);
  doTest(u8R"("\U0000bef")", u8"", false);
  doTest(u8R"("\Ubef")", u8"", false);
  doTest(u8R"("\Ubeef")", u8"", false);
}

TEST(StringParser, practicalString) {
  doTest(u8R"("\r\n")", u8"\r\n", true);
  doTest(u8R"("\033[1m")", u8"\033[1m", true);
}

// Sorry for the macro mess. But it was simpler like that.
#define ENCLOSE_8(literal) u8"\"" literal u8"\""
#define ENCLOSE_16(literal) u"\"" literal u"\""
#define ENCLOSE_32(literal) U"\"" literal U"\""

#define SMALL_BLOCK_8 u8"🆇🆈🆉"
#define SMALL_BLOCK_16 u"🆇🆈🆉"
#define SMALL_BLOCK_32 U"🆇🆈🆉"

// The text has been taken from Wikipedia.
#define SAMPLE_LATIN_8                                                         \
  u8"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod " \
  u8"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "    \
  u8"veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea " \
  u8"commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "  \
  u8"velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint "       \
  u8"occaecat cupidatat non proident, sunt in culpa qui officia deserunt "     \
  u8"mollit anim id est laborum."
#define SAMPLE_LATIN_16                                                       \
  u"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod " \
  u"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "    \
  u"veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea " \
  u"commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "  \
  u"velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint "       \
  u"occaecat cupidatat non proident, sunt in culpa qui officia deserunt "     \
  u"mollit anim id est laborum."
#define SAMPLE_LATIN_32                                                       \
  U"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod " \
  U"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "    \
  U"veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea " \
  U"commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "  \
  U"velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint "       \
  U"occaecat cupidatat non proident, sunt in culpa qui officia deserunt "     \
  U"mollit anim id est laborum."

// Note: this is NO CYRILLIC TRANSLATION. It is just the above text, transcribed
// to cyrillic script (should sound the same as the latin version, but I'm not
// sure), at least if the converter worked.
#define SAMPLE_CYRILLIC_8                                                                                                                \
  u8"Лорэм ипсум долор сит амэт, цонсэцтэтур адиписцинг элит, сэд до эюсмод "  \
  u8"тэмпор инцидидунт ут лаборэ эт долорэ магна алиqуа. Ут эним ад миним "       \
  u8"вэням, qуис ноструд эксэрцитатён улламцо лаборис ниси ут алиqуип экс эа " \
  u8"цоммодо цонсэqуат. Дуис аутэ ирурэ долор ин рэпрэхэндэрит ин волуптатэ " \
  u8"вэлит эссэ циллум долорэ эу фугят нулла парятур. Эксцэптэур синт "            \
  u8"оццаэцат цупидатат нон проидэнт, сунт ин цулпа qуи оффиця дэсэрунт "         \
  u8"моллит аним ид эст лаборум."
#define SAMPLE_CYRILLIC_16                                                                                                              \
  u"Лорэм ипсум долор сит амэт, цонсэцтэтур адиписцинг элит, сэд до эюсмод "  \
  u"тэмпор инцидидунт ут лаборэ эт долорэ магна алиqуа. Ут эним ад миним "       \
  u"вэням, qуис ноструд эксэрцитатён улламцо лаборис ниси ут алиqуип экс эа " \
  u"цоммодо цонсэqуат. Дуис аутэ ирурэ долор ин рэпрэхэндэрит ин волуптатэ " \
  u"вэлит эссэ циллум долорэ эу фугят нулла парятур. Эксцэптэур синт "            \
  u"оццаэцат цупидатат нон проидэнт, сунт ин цулпа qуи оффиця дэсэрунт "         \
  u"моллит аним ид эст лаборум."
#define SAMPLE_CYRILLIC_32                                                                                                              \
  U"Лорэм ипсум долор сит амэт, цонсэцтэтур адиписцинг элит, сэд до эюсмод "  \
  U"тэмпор инцидидунт ут лаборэ эт долорэ магна алиqуа. Ут эним ад миним "       \
  U"вэням, qуис ноструд эксэрцитатён улламцо лаборис ниси ут алиqуип экс эа " \
  U"цоммодо цонсэqуат. Дуис аутэ ирурэ долор ин рэпрэхэндэрит ин волуптатэ " \
  U"вэлит эссэ циллум долорэ эу фугят нулла парятур. Эксцэптэур синт "            \
  U"оццаэцат цупидатат нон проидэнт, сунт ин цулпа qуи оффиця дэсэрунт "         \
  U"моллит аним ид эст лаборум."

// This time, we use one of the math fonts provided with unicode.
#define SAMPLE_EXOTIC_8                                                                                                                                                                                                                                            \
  u8"𝕃𝕠𝕣𝕖𝕞 𝕚𝕡𝕤𝕦𝕞 𝕕𝕠𝕝𝕠𝕣 𝕤𝕚𝕥 𝕒𝕞𝕖𝕥, 𝕔𝕠𝕟𝕤𝕖𝕔𝕥𝕖𝕥𝕦𝕣 𝕒𝕕𝕚𝕡𝕚𝕤𝕔𝕚𝕟𝕘 𝕖𝕝𝕚𝕥, 𝕤𝕖𝕕 𝕕𝕠 𝕖𝕚𝕦𝕤𝕞𝕠𝕕 "    \
  u8"𝕥𝕖𝕞𝕡𝕠𝕣 𝕚𝕟𝕔𝕚𝕕𝕚𝕕𝕦𝕟𝕥 𝕦𝕥 𝕝𝕒𝕓𝕠𝕣𝕖 𝕖𝕥 𝕕𝕠𝕝𝕠𝕣𝕖 𝕞𝕒𝕘𝕟𝕒 𝕒𝕝𝕚𝕢𝕦𝕒. 𝕌𝕥 𝕖𝕟𝕚𝕞 𝕒𝕕 𝕞𝕚𝕟𝕚𝕞 "                \
  u8"𝕧𝕖𝕟𝕚𝕒𝕞, 𝕢𝕦𝕚𝕤 𝕟𝕠𝕤𝕥𝕣𝕦𝕕 𝕖𝕩𝕖𝕣𝕔𝕚𝕥𝕒𝕥𝕚𝕠𝕟 𝕦𝕝𝕝𝕒𝕞𝕔𝕠 𝕝𝕒𝕓𝕠𝕣𝕚𝕤 𝕟𝕚𝕤𝕚 𝕦𝕥 𝕒𝕝𝕚𝕢𝕦𝕚𝕡 𝕖𝕩 𝕖𝕒 " \
  u8"𝕔𝕠𝕞𝕞𝕠𝕕𝕠 𝕔𝕠𝕟𝕤𝕖𝕢𝕦𝕒𝕥. 𝔻𝕦𝕚𝕤 𝕒𝕦𝕥𝕖 𝕚𝕣𝕦𝕣𝕖 𝕕𝕠𝕝𝕠𝕣 𝕚𝕟 𝕣𝕖𝕡𝕣𝕖𝕙𝕖𝕟𝕕𝕖𝕣𝕚𝕥 𝕚𝕟 𝕧𝕠𝕝𝕦𝕡𝕥𝕒𝕥𝕖 "  \
  u8"𝕧𝕖𝕝𝕚𝕥 𝕖𝕤𝕤𝕖 𝕔𝕚𝕝𝕝𝕦𝕞 𝕕𝕠𝕝𝕠𝕣𝕖 𝕖𝕦 𝕗𝕦𝕘𝕚𝕒𝕥 𝕟𝕦𝕝𝕝𝕒 𝕡𝕒𝕣𝕚𝕒𝕥𝕦𝕣. 𝔼𝕩𝕔𝕖𝕡𝕥𝕖𝕦𝕣 𝕤𝕚𝕟𝕥 "                      \
  u8"𝕠𝕔𝕔𝕒𝕖𝕔𝕒𝕥 𝕔𝕦𝕡𝕚𝕕𝕒𝕥𝕒𝕥 𝕟𝕠𝕟 𝕡𝕣𝕠𝕚𝕕𝕖𝕟𝕥, 𝕤𝕦𝕟𝕥 𝕚𝕟 𝕔𝕦𝕝𝕡𝕒 𝕢𝕦𝕚 𝕠𝕗𝕗𝕚𝕔𝕚𝕒 𝕕𝕖𝕤𝕖𝕣𝕦𝕟𝕥 "              \
  u8"𝕞𝕠𝕝𝕝𝕚𝕥 𝕒𝕟𝕚𝕞 𝕚𝕕 𝕖𝕤𝕥 𝕝𝕒𝕓𝕠𝕣𝕦𝕞."
#define SAMPLE_EXOTIC_16                                                                                                                                                                                                                                          \
  u"𝕃𝕠𝕣𝕖𝕞 𝕚𝕡𝕤𝕦𝕞 𝕕𝕠𝕝𝕠𝕣 𝕤𝕚𝕥 𝕒𝕞𝕖𝕥, 𝕔𝕠𝕟𝕤𝕖𝕔𝕥𝕖𝕥𝕦𝕣 𝕒𝕕𝕚𝕡𝕚𝕤𝕔𝕚𝕟𝕘 𝕖𝕝𝕚𝕥, 𝕤𝕖𝕕 𝕕𝕠 𝕖𝕚𝕦𝕤𝕞𝕠𝕕 "    \
  u"𝕥𝕖𝕞𝕡𝕠𝕣 𝕚𝕟𝕔𝕚𝕕𝕚𝕕𝕦𝕟𝕥 𝕦𝕥 𝕝𝕒𝕓𝕠𝕣𝕖 𝕖𝕥 𝕕𝕠𝕝𝕠𝕣𝕖 𝕞𝕒𝕘𝕟𝕒 𝕒𝕝𝕚𝕢𝕦𝕒. 𝕌𝕥 𝕖𝕟𝕚𝕞 𝕒𝕕 𝕞𝕚𝕟𝕚𝕞 "                \
  u"𝕧𝕖𝕟𝕚𝕒𝕞, 𝕢𝕦𝕚𝕤 𝕟𝕠𝕤𝕥𝕣𝕦𝕕 𝕖𝕩𝕖𝕣𝕔𝕚𝕥𝕒𝕥𝕚𝕠𝕟 𝕦𝕝𝕝𝕒𝕞𝕔𝕠 𝕝𝕒𝕓𝕠𝕣𝕚𝕤 𝕟𝕚𝕤𝕚 𝕦𝕥 𝕒𝕝𝕚𝕢𝕦𝕚𝕡 𝕖𝕩 𝕖𝕒 " \
  u"𝕔𝕠𝕞𝕞𝕠𝕕𝕠 𝕔𝕠𝕟𝕤𝕖𝕢𝕦𝕒𝕥. 𝔻𝕦𝕚𝕤 𝕒𝕦𝕥𝕖 𝕚𝕣𝕦𝕣𝕖 𝕕𝕠𝕝𝕠𝕣 𝕚𝕟 𝕣𝕖𝕡𝕣𝕖𝕙𝕖𝕟𝕕𝕖𝕣𝕚𝕥 𝕚𝕟 𝕧𝕠𝕝𝕦𝕡𝕥𝕒𝕥𝕖 "  \
  u"𝕧𝕖𝕝𝕚𝕥 𝕖𝕤𝕤𝕖 𝕔𝕚𝕝𝕝𝕦𝕞 𝕕𝕠𝕝𝕠𝕣𝕖 𝕖𝕦 𝕗𝕦𝕘𝕚𝕒𝕥 𝕟𝕦𝕝𝕝𝕒 𝕡𝕒𝕣𝕚𝕒𝕥𝕦𝕣. 𝔼𝕩𝕔𝕖𝕡𝕥𝕖𝕦𝕣 𝕤𝕚𝕟𝕥 "                      \
  u"𝕠𝕔𝕔𝕒𝕖𝕔𝕒𝕥 𝕔𝕦𝕡𝕚𝕕𝕒𝕥𝕒𝕥 𝕟𝕠𝕟 𝕡𝕣𝕠𝕚𝕕𝕖𝕟𝕥, 𝕤𝕦𝕟𝕥 𝕚𝕟 𝕔𝕦𝕝𝕡𝕒 𝕢𝕦𝕚 𝕠𝕗𝕗𝕚𝕔𝕚𝕒 𝕕𝕖𝕤𝕖𝕣𝕦𝕟𝕥 "              \
  u"𝕞𝕠𝕝𝕝𝕚𝕥 𝕒𝕟𝕚𝕞 𝕚𝕕 𝕖𝕤𝕥 𝕝𝕒𝕓𝕠𝕣𝕦𝕞."
#define SAMPLE_EXOTIC_32                                                                                                                                                                                                                                          \
  U"𝕃𝕠𝕣𝕖𝕞 𝕚𝕡𝕤𝕦𝕞 𝕕𝕠𝕝𝕠𝕣 𝕤𝕚𝕥 𝕒𝕞𝕖𝕥, 𝕔𝕠𝕟𝕤𝕖𝕔𝕥𝕖𝕥𝕦𝕣 𝕒𝕕𝕚𝕡𝕚𝕤𝕔𝕚𝕟𝕘 𝕖𝕝𝕚𝕥, 𝕤𝕖𝕕 𝕕𝕠 𝕖𝕚𝕦𝕤𝕞𝕠𝕕 "    \
  U"𝕥𝕖𝕞𝕡𝕠𝕣 𝕚𝕟𝕔𝕚𝕕𝕚𝕕𝕦𝕟𝕥 𝕦𝕥 𝕝𝕒𝕓𝕠𝕣𝕖 𝕖𝕥 𝕕𝕠𝕝𝕠𝕣𝕖 𝕞𝕒𝕘𝕟𝕒 𝕒𝕝𝕚𝕢𝕦𝕒. 𝕌𝕥 𝕖𝕟𝕚𝕞 𝕒𝕕 𝕞𝕚𝕟𝕚𝕞 "                \
  U"𝕧𝕖𝕟𝕚𝕒𝕞, 𝕢𝕦𝕚𝕤 𝕟𝕠𝕤𝕥𝕣𝕦𝕕 𝕖𝕩𝕖𝕣𝕔𝕚𝕥𝕒𝕥𝕚𝕠𝕟 𝕦𝕝𝕝𝕒𝕞𝕔𝕠 𝕝𝕒𝕓𝕠𝕣𝕚𝕤 𝕟𝕚𝕤𝕚 𝕦𝕥 𝕒𝕝𝕚𝕢𝕦𝕚𝕡 𝕖𝕩 𝕖𝕒 " \
  U"𝕔𝕠𝕞𝕞𝕠𝕕𝕠 𝕔𝕠𝕟𝕤𝕖𝕢𝕦𝕒𝕥. 𝔻𝕦𝕚𝕤 𝕒𝕦𝕥𝕖 𝕚𝕣𝕦𝕣𝕖 𝕕𝕠𝕝𝕠𝕣 𝕚𝕟 𝕣𝕖𝕡𝕣𝕖𝕙𝕖𝕟𝕕𝕖𝕣𝕚𝕥 𝕚𝕟 𝕧𝕠𝕝𝕦𝕡𝕥𝕒𝕥𝕖 "  \
  U"𝕧𝕖𝕝𝕚𝕥 𝕖𝕤𝕤𝕖 𝕔𝕚𝕝𝕝𝕦𝕞 𝕕𝕠𝕝𝕠𝕣𝕖 𝕖𝕦 𝕗𝕦𝕘𝕚𝕒𝕥 𝕟𝕦𝕝𝕝𝕒 𝕡𝕒𝕣𝕚𝕒𝕥𝕦𝕣. 𝔼𝕩𝕔𝕖𝕡𝕥𝕖𝕦𝕣 𝕤𝕚𝕟𝕥 "                      \
  U"𝕠𝕔𝕔𝕒𝕖𝕔𝕒𝕥 𝕔𝕦𝕡𝕚𝕕𝕒𝕥𝕒𝕥 𝕟𝕠𝕟 𝕡𝕣𝕠𝕚𝕕𝕖𝕟𝕥, 𝕤𝕦𝕟𝕥 𝕚𝕟 𝕔𝕦𝕝𝕡𝕒 𝕢𝕦𝕚 𝕠𝕗𝕗𝕚𝕔𝕚𝕒 𝕕𝕖𝕤𝕖𝕣𝕦𝕟𝕥 "              \
  U"𝕞𝕠𝕝𝕝𝕚𝕥 𝕒𝕟𝕚𝕞 𝕚𝕕 𝕖𝕤𝕥 𝕝𝕒𝕓𝕠𝕣𝕦𝕞."

// Last test, this time with the unicode block characters.
#define SAMPLE_BLOCK_8                                                                                                                                                                                                                                             \
  u8"🅻🅾🆁🅴🅼 🅸🅿🆂🆄🅼 🅳🅾🅻🅾🆁 🆂🅸🆃 🅰🅼🅴🆃, 🅲🅾🅽🆂🅴🅲🆃🅴🆃🆄🆁 🅰🅳🅸🅿🅸🆂🅲🅸🅽🅶 🅴🅻🅸🆃, 🆂🅴🅳 🅳🅾 🅴🅸🆄🆂🅼🅾🅳 "    \
  u8"🆃🅴🅼🅿🅾🆁 🅸🅽🅲🅸🅳🅸🅳🆄🅽🆃 🆄🆃 🅻🅰🅱🅾🆁🅴 🅴🆃 🅳🅾🅻🅾🆁🅴 🅼🅰🅶🅽🅰 🅰🅻🅸🆀🆄🅰. 🆄🆃 🅴🅽🅸🅼 🅰🅳 🅼🅸🅽🅸🅼 "                \
  u8"🆅🅴🅽🅸🅰🅼, 🆀🆄🅸🆂 🅽🅾🆂🆃🆁🆄🅳 🅴🆇🅴🆁🅲🅸🆃🅰🆃🅸🅾🅽 🆄🅻🅻🅰🅼🅲🅾 🅻🅰🅱🅾🆁🅸🆂 🅽🅸🆂🅸 🆄🆃 🅰🅻🅸🆀🆄🅸🅿 🅴🆇 🅴🅰 " \
  u8"🅲🅾🅼🅼🅾🅳🅾 🅲🅾🅽🆂🅴🆀🆄🅰🆃. 🅳🆄🅸🆂 🅰🆄🆃🅴 🅸🆁🆄🆁🅴 🅳🅾🅻🅾🆁 🅸🅽 🆁🅴🅿🆁🅴🅷🅴🅽🅳🅴🆁🅸🆃 🅸🅽 🆅🅾🅻🆄🅿🆃🅰🆃🅴 "  \
  u8"🆅🅴🅻🅸🆃 🅴🆂🆂🅴 🅲🅸🅻🅻🆄🅼 🅳🅾🅻🅾🆁🅴 🅴🆄 🅵🆄🅶🅸🅰🆃 🅽🆄🅻🅻🅰 🅿🅰🆁🅸🅰🆃🆄🆁. 🅴🆇🅲🅴🅿🆃🅴🆄🆁 🆂🅸🅽🆃 "                      \
  u8"🅾🅲🅲🅰🅴🅲🅰🆃 🅲🆄🅿🅸🅳🅰🆃🅰🆃 🅽🅾🅽 🅿🆁🅾🅸🅳🅴🅽🆃, 🆂🆄🅽🆃 🅸🅽 🅲🆄🅻🅿🅰 🆀🆄🅸 🅾🅵🅵🅸🅲🅸🅰 🅳🅴🆂🅴🆁🆄🅽🆃 "              \
  u8"🅼🅾🅻🅻🅸🆃 🅰🅽🅸🅼 🅸🅳 🅴🆂🆃 🅻🅰🅱🅾🆁🆄🅼."
#define SAMPLE_BLOCK_16                                                                                                                                                                                                                                           \
  u"🅻🅾🆁🅴🅼 🅸🅿🆂🆄🅼 🅳🅾🅻🅾🆁 🆂🅸🆃 🅰🅼🅴🆃, 🅲🅾🅽🆂🅴🅲🆃🅴🆃🆄🆁 🅰🅳🅸🅿🅸🆂🅲🅸🅽🅶 🅴🅻🅸🆃, 🆂🅴🅳 🅳🅾 🅴🅸🆄🆂🅼🅾🅳 "    \
  u"🆃🅴🅼🅿🅾🆁 🅸🅽🅲🅸🅳🅸🅳🆄🅽🆃 🆄🆃 🅻🅰🅱🅾🆁🅴 🅴🆃 🅳🅾🅻🅾🆁🅴 🅼🅰🅶🅽🅰 🅰🅻🅸🆀🆄🅰. 🆄🆃 🅴🅽🅸🅼 🅰🅳 🅼🅸🅽🅸🅼 "                \
  u"🆅🅴🅽🅸🅰🅼, 🆀🆄🅸🆂 🅽🅾🆂🆃🆁🆄🅳 🅴🆇🅴🆁🅲🅸🆃🅰🆃🅸🅾🅽 🆄🅻🅻🅰🅼🅲🅾 🅻🅰🅱🅾🆁🅸🆂 🅽🅸🆂🅸 🆄🆃 🅰🅻🅸🆀🆄🅸🅿 🅴🆇 🅴🅰 " \
  u"🅲🅾🅼🅼🅾🅳🅾 🅲🅾🅽🆂🅴🆀🆄🅰🆃. 🅳🆄🅸🆂 🅰🆄🆃🅴 🅸🆁🆄🆁🅴 🅳🅾🅻🅾🆁 🅸🅽 🆁🅴🅿🆁🅴🅷🅴🅽🅳🅴🆁🅸🆃 🅸🅽 🆅🅾🅻🆄🅿🆃🅰🆃🅴 "  \
  u"🆅🅴🅻🅸🆃 🅴🆂🆂🅴 🅲🅸🅻🅻🆄🅼 🅳🅾🅻🅾🆁🅴 🅴🆄 🅵🆄🅶🅸🅰🆃 🅽🆄🅻🅻🅰 🅿🅰🆁🅸🅰🆃🆄🆁. 🅴🆇🅲🅴🅿🆃🅴🆄🆁 🆂🅸🅽🆃 "                      \
  u"🅾🅲🅲🅰🅴🅲🅰🆃 🅲🆄🅿🅸🅳🅰🆃🅰🆃 🅽🅾🅽 🅿🆁🅾🅸🅳🅴🅽🆃, 🆂🆄🅽🆃 🅸🅽 🅲🆄🅻🅿🅰 🆀🆄🅸 🅾🅵🅵🅸🅲🅸🅰 🅳🅴🆂🅴🆁🆄🅽🆃 "              \
  u"🅼🅾🅻🅻🅸🆃 🅰🅽🅸🅼 🅸🅳 🅴🆂🆃 🅻🅰🅱🅾🆁🆄🅼."
#define SAMPLE_BLOCK_32                                                                                                                                                                                                                                           \
  U"🅻🅾🆁🅴🅼 🅸🅿🆂🆄🅼 🅳🅾🅻🅾🆁 🆂🅸🆃 🅰🅼🅴🆃, 🅲🅾🅽🆂🅴🅲🆃🅴🆃🆄🆁 🅰🅳🅸🅿🅸🆂🅲🅸🅽🅶 🅴🅻🅸🆃, 🆂🅴🅳 🅳🅾 🅴🅸🆄🆂🅼🅾🅳 "    \
  U"🆃🅴🅼🅿🅾🆁 🅸🅽🅲🅸🅳🅸🅳🆄🅽🆃 🆄🆃 🅻🅰🅱🅾🆁🅴 🅴🆃 🅳🅾🅻🅾🆁🅴 🅼🅰🅶🅽🅰 🅰🅻🅸🆀🆄🅰. 🆄🆃 🅴🅽🅸🅼 🅰🅳 🅼🅸🅽🅸🅼 "                \
  U"🆅🅴🅽🅸🅰🅼, 🆀🆄🅸🆂 🅽🅾🆂🆃🆁🆄🅳 🅴🆇🅴🆁🅲🅸🆃🅰🆃🅸🅾🅽 🆄🅻🅻🅰🅼🅲🅾 🅻🅰🅱🅾🆁🅸🆂 🅽🅸🆂🅸 🆄🆃 🅰🅻🅸🆀🆄🅸🅿 🅴🆇 🅴🅰 " \
  U"🅲🅾🅼🅼🅾🅳🅾 🅲🅾🅽🆂🅴🆀🆄🅰🆃. 🅳🆄🅸🆂 🅰🆄🆃🅴 🅸🆁🆄🆁🅴 🅳🅾🅻🅾🆁 🅸🅽 🆁🅴🅿🆁🅴🅷🅴🅽🅳🅴🆁🅸🆃 🅸🅽 🆅🅾🅻🆄🅿🆃🅰🆃🅴 "  \
  U"🆅🅴🅻🅸🆃 🅴🆂🆂🅴 🅲🅸🅻🅻🆄🅼 🅳🅾🅻🅾🆁🅴 🅴🆄 🅵🆄🅶🅸🅰🆃 🅽🆄🅻🅻🅰 🅿🅰🆁🅸🅰🆃🆄🆁. 🅴🆇🅲🅴🅿🆃🅴🆄🆁 🆂🅸🅽🆃 "                      \
  U"🅾🅲🅲🅰🅴🅲🅰🆃 🅲🆄🅿🅸🅳🅰🆃🅰🆃 🅽🅾🅽 🅿🆁🅾🅸🅳🅴🅽🆃, 🆂🆄🅽🆃 🅸🅽 🅲🆄🅻🅿🅰 🆀🆄🅸 🅾🅵🅵🅸🅲🅸🅰 🅳🅴🆂🅴🆁🆄🅽🆃 "              \
  U"🅼🅾🅻🅻🅸🆃 🅰🅽🅸🅼 🅸🅳 🅴🆂🆃 🅻🅰🅱🅾🆁🆄🅼."

#define BUILD_TEST_CHRSIZE(text, size1, size2) \
  doTest(ENCLOSE_##size1(text##_##size1), text##_##size2, true)

#define BUILD_TEST_CHRSIZE_MATRIX(text) \
  BUILD_TEST_CHRSIZE(text, 8, 8);       \
  BUILD_TEST_CHRSIZE(text, 8, 16);      \
  BUILD_TEST_CHRSIZE(text, 8, 32);      \
                                        \
  BUILD_TEST_CHRSIZE(text, 16, 8);      \
  BUILD_TEST_CHRSIZE(text, 16, 16);     \
  BUILD_TEST_CHRSIZE(text, 16, 32);     \
                                        \
  BUILD_TEST_CHRSIZE(text, 32, 8);      \
  BUILD_TEST_CHRSIZE(text, 32, 16);     \
  BUILD_TEST_CHRSIZE(text, 32, 32);

TEST(StringParser, decodeDifferentCharacterSizes) {
  BUILD_TEST_CHRSIZE_MATRIX(SAMPLE_LATIN);
}

TEST(StringParser, decodeDifferentCharacterSizes2) {
  BUILD_TEST_CHRSIZE_MATRIX(SAMPLE_CYRILLIC);
}

TEST(StringParser, decodeDifferentCharacterSizes3) {
  BUILD_TEST_CHRSIZE_MATRIX(SAMPLE_EXOTIC);
}

TEST(StringParser, decodeDifferentCharacterSizes4) {
  BUILD_TEST_CHRSIZE_MATRIX(SAMPLE_BLOCK);
}

TEST(StringParser, decodeDifferentCharacterSizesSimple) {
  BUILD_TEST_CHRSIZE_MATRIX(SMALL_BLOCK);
}
