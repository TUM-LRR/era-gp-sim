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

#ifndef ERAGPSIM_PARSER_STRING_PARSER_HPP_
#define ERAGPSIM_PARSER_STRING_PARSER_HPP_

#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "common/utility.hpp"
#include "parser/compile-state.hpp"

/**
 * \brief Provides help methods for parsing strings.
 *
 * This class is actually concepted only for internal use, but it can decode and
 * re-encode code-points written in a C-like string, if only provided with the
 * right size. If one type is 1 byte big, it uses UTF-8 as a format, if 2 or 3
 * bytes big, it uses UTF-16, and if 4 or bigger bytes used, it chooses UTF-32.
 *
 * \tparam CharT The input character type.
 * \tparam OutT The output data type. (does not need to be something
 * character-like, but if you want UTF-32 encoded in bytes, you got to put out
 * in uint32_t and then convert to bytes yourself)
 */
template <typename CharT, typename OutT>
class StringParserEngine {
 public:
  using CharType = Utility::TypeBarrier<CharT, std::is_integral>;
  using OutType = Utility::TypeBarrier<OutT, std::is_integral>;
  using String = std::basic_string<CharType>;
  using Output = std::vector<OutType>;

  /**
   * \brief Decodes and re-encodes a code point at the given position in the
   * input string.
   * \param inputString The input string to take the data from.
   * \param index The index at which the to-be-encoded data begins.
   * \param separator The separator (e.g. " or ') to indicate the end or begin
   * of a string.
   * \param output The data output to append data to.
   * \param state The compile state to write errors to.
   * \return True, if the conversion has been successful. If not, it returns
   * false.
   */
  static bool stringParseCharacter(const String& inputString,
                                   size_t& index,
                                   char separator,
                                   Output& output,
                                   CompileState& state) {
    auto chr = inputString[index];
    if (chr == separator) {
      // No Ansi C-like string concatenation, yet. Sorry!
      invokeError("Found an unescaped separator in a string", index, state);
      return false;
    } else {
      // If we do not have a separator: we convert the character!
      if (!stringConvertCharacter(inputString, index, output, state)) {
        return false;
      }
    }

    // Nothing wrong, everything has been ok!
    return true;
  }

  /**
   * \brief Checks, if a string is wrapped by two separators.
   * \param inputString The string to check.
   * \param separator The separator which should wrap the string.
   * \param state The compile state to note down any errors.
   * \return True, if the string is wrapped. Else, it returns false.
   */
  static bool checkIfWrapped(const String& inputString,
                             char separator,
                             CompileState& state) {
    // If the string is too small, it cannot even contain the two brackets.
    if (inputString.size() < 2) {
      invokeError("The supposed string literal is too small!", 0, state);
      return false;
    }

    // We check the beginning of the string...
    if (inputString[0] != separator) {
      invokeError(
          std::string(
              "Something supposed to be a string does not begin with ") +
              separator,
          0,
          state);
      return false;
    }

    //...and the end.
    if (inputString[inputString.size() - 1] != separator) {
      invokeError(
          std::string("Something supposed to be a string does not end with ") +
              separator,
          inputString.size() - 1,
          state);
      return false;
    }

    return true;
  }

 private:
  // This method notes down an error in the given compile state.
  static void invokeError(const std::string& message,
                          size_t position,
                          CompileState& state) {
    state.addError(message, state.position >> position);
  }

  // Returns true, if there is still data after the current index in the string
  // and increments the index.
  static bool requireCharacter(const String& inputString, size_t& index) {
    ++index;
    return index < inputString.size() - 1;
  }

  // Decodes a UTF-8 code point at the given position.
  static bool decodeUTF8CodePoint(const String& inputString,
                                  size_t& index,
                                  uint32_t& codePoint,
                                  CompileState& state) {
    auto chr = inputString[index];
    if ((chr & 0x80) == 0) {
      // If the most significant bit is a 0, we got an ASCII character and can
      // just return it.
      codePoint = chr;
      return true;
    } else {
      // If not, we got a compound of two to four (theoretically six) bytes.
      int size = 0;
      uint32_t value = 0;
      auto wchr = chr;

      // Format of the first byte is: 1..10<DATA>, the number of trailing ones
      // equals the number of bytes the code point is encoded in. For this case,
      // the number of trailing ones cannot be just one. (i.e. 10<DATA>)
      while ((wchr & 0x40) != 0) {
        if (!requireCharacter(inputString, index)) {
          // If we cannot find another byte, but we need it, we are screwed.
          invokeError("Reached end of string with unfinished code point.",
                      index,
                      state);
          return false;
        }

        // So, we have found another byte, we have to check that it is no
        // erroneous encoded one.
        auto nchr = inputString[index];

        if ((nchr & 0xc0) != 0x80) {
          invokeError("Erroneous in-code point character.", index, state);
          return false;
        }

        // All checks done, let's add its data to the code point value. We got 6
        // bits of data in a byte.
        value <<= 6;
        value |= nchr & 0x3f;

        ++size;

        wchr <<= 1;
      }

      // As stated before 10<DATA> is an in-code point byte, so if we have size
      // 0 (b/c we check the second most significant byte), we got an error.
      if (size == 0) {
        invokeError("Invalid-formed code point detected!", index, state);
        return false;
      } else {
        // If not, we need to get the rest of data out of the first character,
        // and add it to our value. The amount of data bits varies, the more
        // data bytes, the less data bits in the starting byte.
        uint32_t rest = chr & ((1 << (6 - size)) - 1);
        value |= rest << (size * 6);
        codePoint = value;
        if (codePoint > 0x10ffff) {
          // As there can be higher values than specified in Unicode, we need to
          // catch these too.
          invokeError("The specified code point is outside the Unicode range!",
                      index,
                      state);
          return false;
        }
        return true;
      }
    }
  }

  // Decodes a UTF-16 code point.
  static bool decodeUTF16CodePoint(const String& inputString,
                                   size_t& index,
                                   uint32_t& codePoint,
                                   CompileState& state) {
    auto chr = inputString[index] & 0xffff;
    if (chr <= 0xd7ff || chr >= 0xe000) {
      // In this case, we got a normal one-short sized code point.
      codePoint = chr;
      return true;
    } else {
      if ((chr & 0xfc00) != 0xd800) {
        // If we got a two-short sized code point, the first needs to be an
        // upper surrogate point.
        invokeError(
            "Invalid-formed code point detected (one-short-sized code point "
            "does not begin with a high surrogate character)!",
            index,
            state);
        return false;
      }

      if (!requireCharacter(inputString, index)) {
        // If there is no second short in the string, throw an error, b/c we
        // still need one.
        invokeError(
            "End of string detecten while decoding code point!", index, state);
        return false;
      }

      auto lchr = inputString[index];

      if ((lchr & 0xfc00) != 0xdc00) {
        // The second of the two shorts needs to be a lower surrogate.
        invokeError(
            "Invalid-formed code point detected (second short of code point is "
            "not a low surrogate character)!",
            index,
            state);
        return false;
      }

      // Now take the 10 bits from each surrogate, align them and add 65536.
      uint32_t hvalue = chr & 0x3ff;
      uint32_t lvalue = lchr & 0x3ff;
      codePoint = ((hvalue << 10) | lvalue) + 0x10000;
      return true;
    }
  }

  // Decodes a UTF-32 code point.
  static bool decodeUTF32CodePoint(const String& inputString,
                                   size_t& index,
                                   uint32_t& codePoint,
                                   CompileState& state) {
    // This is by far the easiest decode. We take the value from the stream and
    // take it as code point, but only if it is inside the Unicode
    // specification.
    codePoint = inputString[index];
    if (codePoint > 0x10ffff) {
      invokeError("The specified code point is outside the Unicode range!",
                  index,
                  state);
      return false;
    }
    return true;
  }

  // Decodes a code point according the input string data size.
  static bool decodeCodePoint(const String& inputString,
                              size_t& index,
                              uint32_t& codePoint,
                              CompileState& state) {
    // We got a sizeof-decision tree or so here...
    if (sizeof(CharType) >= 4) {
      return decodeUTF32CodePoint(inputString, index, codePoint, state);
    } else if (sizeof(CharType) >= 2) {
      return decodeUTF16CodePoint(inputString, index, codePoint, state);
    } else if (sizeof(CharType) == 1) {
      return decodeUTF8CodePoint(inputString, index, codePoint, state);
    } else {
      return false;
    }
  }

  // Helper method for checking if a character can be treated as a hexadecimal
  // value.
  static bool isHex(CharType chr) {
    return (chr >= '0' && chr <= '9') || (chr >= 'A' && chr <= 'F') ||
           (chr >= 'a' && chr <= 'f');
  }

  // Helper method for checking if a character can be treated as an octal value.
  static bool isOctal(CharType chr) {
    return chr >= '0' && chr <= '7';
  }

  // This method gets at most maxLength characters which comply to the given
  // rangeCheck. It also increments the index.
  static int crawlIndex(const String& inputString,
                        size_t& index,
                        std::function<bool(char)> rangeCheck,
                        int maxLength) {
    int length = 0;
    for (int i = 0; i < maxLength; ++i) {
      if (!(requireCharacter(inputString, index) &&
            rangeCheck(inputString[index]))) {
        // If there is no more character or the character is not as expected,
        // abort.
        break;
      }
      ++length;
    }
    if (length == maxLength) {
      // Fixing a small bug, we want to point the index to after the sequence.
      ++index;
    }
    return length;
  }

  // This is a quickly-written replacement for stoi, working on arbitrary
  // strings.
  template <typename T>
  static T simpleNumberParse(const String& inputString,
                             size_t start,
                             size_t length,
                             int base) {
    T value = 0;
    for (auto i = inputString.begin() + start;
         i != inputString.begin() + start + length;
         ++i) {
      // We increment the base, then we decode the character.
      value *= base;
      auto chr = *i;
      if (chr >= '0' && chr <= '9') {
        value += (T)(chr - '0');
      } else if (chr >= 'A' && chr <= 'Z') {
        value += (T)(chr - 'A') + 10;
      } else if (chr >= 'a' && chr <= 'z') {
        value += (T)(chr - 'a') + 10;
      }
    }
    return value;
  }

  // Parses an escape sequence encoding a Unicode code point.
  static bool parseUnicodeEscapeSequence(const String& inputString,
                                         size_t& index,
                                         Output& output,
                                         int size,
                                         CompileState& state) {
    size_t startIndex = index;

    auto len = crawlIndex(inputString, index, isHex, size);

    // Important: The size of the specified index in the string must equal to
    // the maximum size.
    if (len != size) {
      invokeError("Not the right size for the escape sequence", index, state);
      return false;
    }

    // Then we get and encode the code point.
    uint32_t codePoint =
        simpleNumberParse<uint32_t>(inputString, startIndex + 1, len, 16);
    return encodeCodePoint(inputString, codePoint, index, output, state);
  }

  // Gets the corresponding character for a simple one-byte long escape
  // sequence, such as \n .
  static int parseOneByteEscapeSequence(char chr) {
    int match;

    // Just a switch case.
    switch (chr) {
      // clang-format off
      case 'a': match  = '\a'; break;
      case 'b': match  = '\b'; break;
      case 'e': match  = '\x1b'; break;
      case 'f': match  = '\f'; break;
      case 'n': match  = '\n'; break;
      case 'r': match  = '\r'; break;
      case 't': match  = '\t'; break;
      case 'v': match  = '\v'; break;
      case '?': match  = '\?'; break;
      case '\"': match = '\"'; break;
      case '\'': match = '\''; break;
      case '\\': match = '\\'; break;
      default: match = -1; break;
        // clang-format on
    }

    return match;
  }

  // This methods decodes a C-like escape character.
  static bool parseEscapeCharacter(const String& inputString,
                                   size_t& index,
                                   Output& output,
                                   CompileState& state) {
    if (!requireCharacter(inputString, index)) {
      // A \ followed by an end of string is not valid.
      invokeError("Unfinished escape sequence!", index, state);
      return false;
    }

    size_t startIndex = index;
    auto chr = inputString[index];
    char cchr = (char)chr;

    int match = parseOneByteEscapeSequence(cchr);
    if (match != -1) {
      // One-byte escape sequence.
      output.push_back(match);
      ++index;
      return true;
    } else if (chr == 'u') {
      // Decoding a Unicode code point in the range  up to 65535.
      return parseUnicodeEscapeSequence(inputString, index, output, 4, state);
    } else if (chr == 'U') {
      // Decoding an arbitrary Unicode code point.
      return parseUnicodeEscapeSequence(inputString, index, output, 8, state);
    } else if (chr == 'x') {
      // Decoding a unit-sized character, ignoring all UTF alignments.
      auto len = crawlIndex(inputString, index, isHex, sizeof(OutType) * 2);
      OutType value =
          simpleNumberParse<OutType>(inputString, startIndex + 1, len, 16);
      output.push_back(value);
    } else if (isOctal(chr)) {
      // Decoding an octal character sequence. (e.g. special case is: \0, but
      // can also be \000)
      auto len = crawlIndex(inputString, index, isOctal, 2) + 1;
      int ret = simpleNumberParse<int>(inputString, startIndex, len, 8);
      if (ret > 0xff && sizeof(OutType) == 1) {
        invokeError(
            "The specified octal sequence wants to encode a character out "
            "of byte size.",
            index,
            state);
        return false;
      }
      output.push_back(ret);
    } else {
      // If nothing of the above applies, we got an invalid escape sequence.
      invokeError("Detected invalid escape sequence!", index, state);
      return false;
    }
    return true;
  }

  // The central method to convert a character.
  static bool stringConvertCharacter(const String& inputString,
                                     size_t& index,
                                     Output& output,
                                     CompileState& state) {
    auto chr = inputString[index];
    if (chr == '\\') {
      // This is, where the fun begins! Escape character parsing... Yay!
      if (!parseEscapeCharacter(inputString, index, output, state)) {
        return false;
      }
    } else {
      // If not, we just convert source to target encoding.
      uint32_t codePoint;
      if (!decodeCodePoint(inputString, index, codePoint, state)) {
        return false;
      }
      if (!encodeCodePoint(inputString, codePoint, index, output, state)) {
        return false;
      }
      ++index;
    }
    return true;
  }

  // Encodes a UTF-8 code point.
  static bool encodeUTF8CodePoint(const String& inputString,
                                  uint32_t codePoint,
                                  Output& output,
                                  CompileState& state) {
    if (codePoint <= 0x7f) {
      // If we are in ascii range, there is no need to encode more.
      output.push_back(codePoint);
    } else {
      // If not, we encode the bytes are required.
      int bytes = 1;
      int restSize = 6;
      while (codePoint >= (1 << restSize)) {
        // As long as we got not enough space in the final byte, we got to add
        // another one.
        output.push_back(0x80 | (codePoint & 0x3f));
        codePoint >>= 6;
        --restSize;
        ++bytes;
      }
      // Then, we add the rest byte with the mask as needed, then we still need
      // to reverse the whole sequence as the 'rest' byte has to be the first in
      // the stream.
      int mask = ~((1 << (restSize + 1)) - 1);
      output.push_back(mask | codePoint);
      std::reverse(output.end() - bytes, output.end());
    }
    return true;
  }

  // Encodes a UTF-16 code point.
  static bool encodeUTF16CodePoint(const String& inputString,
                                   uint32_t codePoint,
                                   Output& output,
                                   CompileState& state) {
    if (codePoint <= 0xffff) {
      // We got a one-short code point.
      output.push_back((uint16_t)codePoint);
    } else {
      // If not, we need two. Just subtract 65536, then split into two 10-bit
      // large values, then add the surrogate patterns and add to the stream.
      codePoint -= 0x10000;
      uint16_t upper = (uint32_t)(0xd800 | ((codePoint >> 10) & 0x3ff));
      uint16_t lower = (uint32_t)(0xdc00 | (codePoint & 0x3ff));
      output.push_back(upper);
      output.push_back(lower);
    }
    return true;
  }

  // Encodes a UTF-32 code point.
  static bool encodeUTF32CodePoint(const String& inputString,
                                   uint32_t codePoint,
                                   Output& output,
                                   CompileState& state) {
    // Trivial. ;)
    output.push_back(codePoint);
    return true;
  }

  // Encodes a code point.
  static bool encodeCodePoint(const String& inputString,
                              uint32_t codePoint,
                              size_t& index,
                              Output& output,
                              CompileState& state) {
    if (codePoint > 0x10ffff || (codePoint >= 0xd800 && codePoint <= 0xdfff)) {
      // First of all, we cross out all out of range and surrogate code points,
      // then we do not need to check in the single methods any more.
      invokeError("The specified code point is outside the Unicode range!",
                  index,
                  state);
      return false;
    }

    // Another sizeof discambigulation.
    if (sizeof(OutType) >= 4) {
      return encodeUTF32CodePoint(inputString, codePoint, output, state);
    } else if (sizeof(OutType) >= 2) {
      return encodeUTF16CodePoint(inputString, codePoint, output, state);
    } else if (sizeof(OutType) == 1) {
      return encodeUTF8CodePoint(inputString, codePoint, output, state);
    } else {
      return false;
    }
  }
};

/**
 * \brief Provides some basic methods for string parsing.
 */
namespace StringParser {
/**
* \brief Parses a trimmed C-like string literal in UTF format.
* \tparam CharT The input character type.
* \tparam OutT The output integer type.
* \param inputString The input data.
* \param output The output vector to append to.
* \param state The compile state to note errors down.
* \return True, if the conversion has been successful, else false.
*/
template <typename CharT, typename OutT>
static bool parseString(const std::basic_string<CharT>& inputString,
                        std::vector<OutT>& output,
                        CompileState& state) {
  using CharType = Utility::TypeBarrier<CharT, std::is_integral>;
  using OutType = Utility::TypeBarrier<OutT, std::is_integral>;

  if (!StringParserEngine<CharType, OutType>::checkIfWrapped(
          inputString, '\"', state)) {
    return false;
  }

  size_t index = 1;
  while (inputString.size() - 1 > index) {
    if (!StringParserEngine<CharType, OutType>::stringParseCharacter(
            inputString, index, '\"', output, state)) {
      return false;
    }
  }

  return index == inputString.size() - 1;
}

/**
* \brief Parses a trimmed C-like character literal in UTF format.
* \tparam CharT The input character type.
* \tparam OutT The output integer type.
* \param inputString The input data.
* \param output The output vector to append to.
* \param state The compile state to note errors down.
* \return True, if the conversion has been successful, else false.
*/
template <typename CharT, typename OutT>
static bool parseCharacter(const std::basic_string<CharT>& inputString,
                           std::vector<OutT>& output,
                           CompileState& state) {
  using CharType = Utility::TypeBarrier<CharT, std::is_integral>;
  using OutType = Utility::TypeBarrier<OutT, std::is_integral>;

  if (!StringParserEngine<CharType, OutType>::checkIfWrapped(
          inputString, '\'', state)) {
    return false;
  }

  size_t index = 1;
  if (!StringParserEngine<CharType, OutType>::stringParseCharacter(
          inputString, index, '\'', output, state)) {
    return false;
  }

  return index == inputString.size() - 1;
}
};

#endif /* ERAGPSIM_PARSER_STRING_PARSER_HPP_ */
