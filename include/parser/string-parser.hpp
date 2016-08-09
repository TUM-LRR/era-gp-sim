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

#include "compile-state.hpp"
#include <string>
#include <vector>
#include <functional>
#include <algorithm>

/**
 * \internal
 */
template<typename CharType, typename OutType>
class StringParserInternal {
public:
    using String = std::basic_string<CharType>;
    using Output = std::vector<OutType>;

    static void invokeError(const std::string& message, size_t position, CompileState& state, CompileErrorSeverity severity = CompileErrorSeverity::ERROR)
    {
        auto newPosition = state.position;
        newPosition.second += position;
        state.errorList.push_back(CompileError(message, newPosition, severity));
    }

    static bool checkIfWrapped(const String& inputString, char separator, CompileState& state)
    {
        if (inputString.size() < 2)
        {
            invokeError("The supposed string literal is too small!", 0, state);
            return false;
        }

        if (inputString[0] != separator)
        {
            invokeError(std::string("Something supposed to be a string does not begin with ") + separator, 0, state);
            return false;
        }

        if (inputString[inputString.size() - 1] != separator)
        {
            invokeError(std::string("Something supposed to be a string does not end with ") + separator, inputString.size() - 1, state);
            return false;
        }

        return true;
    }

    static bool requireCharacter(const String& inputString, size_t& index)
    {
        ++index;
        return index < inputString.size() - 1;
    }

    static bool decodeUTF8CodePoint(const String& inputString, size_t& index, uint32_t& codePoint, CompileState& state)
    {
        auto chr = inputString[index];
        if ((chr & 0x80) == 0)
        {
            codePoint = chr;
            return true;
        }
        else
        {            
            int size = 0;
            uint32_t value = 0;
            auto wchr = chr;
            while ((wchr & 0x40) != 0)
            {
                if (!requireCharacter(inputString, index))
                {
                    invokeError("Reached end of string with unfinished code point.", index, state);
                    return false;
                }

                auto nchr = inputString[index];

                if ((nchr & 0xc0) != 0x80)
                {
                    invokeError("Erroneous in-code point character.", index, state);
                    return false;
                }

                value <<= 6;
                value |= nchr & 0x3f;

                ++size;

                wchr <<= 1;
            }

            if (size == 0)
            {
                invokeError("Invalid formed code point detected!", index, state);
                return false;
            }
            else
            {
                uint32_t rest = chr & ((1 << (6 - size)) - 1);
                value |= rest << (size * 6);
                codePoint = value;
                if (codePoint > 0x10ffff)
                {
                    //ERROR
                    return false;
                }
                return true;
            }
        }
    }

    static bool decodeUTF16CodePoint(const String& inputString, size_t& index, uint32_t& codePoint, CompileState& state)
    {
        auto chr = inputString[index] & 0xffff;
        if (chr <= 0xd7ff || chr >= 0xe000)
        {
            codePoint = chr;
            return true;
        }
        else
        {
            if ((chr & 0xfc00) == 0xd8)
            {
                //ERROR
                return false;
            }

            if (!requireCharacter(inputString, index))
            {
                //ERROR
                return false;
            }

            auto lchr = inputString[index];

            if ((chr & 0xfc00) != 0xdc)
            {
                //ERROR
                return false;
            }

            uint32_t hvalue = chr & 0x3ff;
            uint32_t lvalue = chr & 0x3ff;
            codePoint = ((hvalue << 10) | lvalue) + 0x10000;
            return true;
        }
    }

    static bool decodeUTF32CodePoint(const String& inputString, size_t& index, uint32_t& codePoint, CompileState& state)
    {
        codePoint = inputString[index];
        if (codePoint > 0x10ffff)
        {
            //ERROR
            return false;
        }
        return true;
    }

    static bool decodeCodePoint(const String& inputString, size_t& index, uint32_t& codePoint, CompileState& state)
    {
        if (sizeof(CharType) >= 4)
        {
            return decodeUTF32CodePoint(inputString, index, codePoint, state);
        }
        else if (sizeof(CharType) >= 2)
        {
            return decodeUTF16CodePoint(inputString, index, codePoint, state);
        }
        else if (sizeof(CharType) == 1)
        {
            return decodeUTF8CodePoint(inputString, index, codePoint, state);
        }
        else
        {
            return false;
        }
    }

    static bool isHex(CharType chr)
    {
        return (chr >= '0' && chr <= '9') || (chr >= 'A' && chr <= 'F') || (chr >= 'a' && chr <= 'f');
    }

    static bool isOctal(CharType chr)
    {
        return chr >= '0' && chr <= '7';
    }

    static int crawlIndex(const String& inputString, size_t& index, std::function<bool(char)> rangeCheck, int maxLength)
    {
        int length = 0;
        for (int i = 0; i < maxLength || maxLength == -1; ++i)
        {
            if (!(requireCharacter(inputString, index) && rangeCheck(inputString[index])))
            {
                break;
            }
            ++length;
        }
        if (length == maxLength)
        {
            ++index;
        }
        return length;
    }

    static bool parseEscapeCharacter(const String& inputString, size_t& index, Output& output, CompileState& state)
    {
        if (!requireCharacter(inputString, index))
        {
            //ERROR
            return false;
        }

        size_t startIndex = index;
        auto chr = inputString[index];

        switch(chr)
        {
            case 'a': output.push_back('\a'); ++index; break;
            case 'b': output.push_back('\b'); ++index; break;
            case 'e': output.push_back('\e'); ++index; break;
            case 'f': output.push_back('\f'); ++index; break;
            case 'n': output.push_back('\n'); ++index; break;
            case 'r': output.push_back('\r'); ++index; break;
            case 't': output.push_back('\t'); ++index; break;
            case 'v': output.push_back('\v'); ++index; break;
            case '?': output.push_back('?'); ++index; break;
            case '\'': output.push_back('\''); ++index; break;
            case '\"': output.push_back('\"'); ++index; break;
            case '\\': output.push_back('\\'); ++index; break;
            case 'x':
            {
                auto len = crawlIndex(inputString, index, isHex, sizeof(OutType) * 2);
                OutType value = std::stol(inputString.substr(startIndex + 1, len), nullptr, 16);
                output.push_back(value);
            }
            break;
            case 'u':
            {
                auto len = crawlIndex(inputString, index, isHex, 4);
                if (len != 4)
                {
                    return false;
                }
                uint32_t codePoint = std::stol(inputString.substr(startIndex + 1, len), nullptr, 16);
                return encodeCodePoint(inputString, codePoint, output, state);
            }
            break;
            case 'U':
            {
                auto len = crawlIndex(inputString, index, isHex, 8);
                if (len != 8)
                {
                    return false;
                }
                uint32_t codePoint = std::stol(inputString.substr(startIndex + 1, len), nullptr, 16);
                return encodeCodePoint(inputString, codePoint, output, state);
            }
            break;
            default:
                if (isOctal(chr))
                {
                    auto len = crawlIndex(inputString, index, isOctal, 2) + 1;
                    int ret = std::stoi(inputString.substr(startIndex, len), nullptr, 8);
                    if (ret > 0xff && sizeof(OutType) == 1)
                    {
                        //ERROR
                        return false;
                    }
                    output.push_back(ret);
                }
                else
                {
                    //ERROR
                    return false;
                }
                break;
        }
        return true;
    }

    static bool stringConvertCharacter(const String& inputString, size_t& index, Output& output, CompileState& state)
    {
        auto chr = inputString[index];
        if (chr == '\\')
        {
            //This is, where the fun begins!
            if (!parseEscapeCharacter(inputString, index, output, state))
            {
                return false;
            }
        }
        else
        {
            uint32_t codePoint;
            if (!decodeCodePoint(inputString, index, codePoint, state))
            {
                return false;
            }
            if (!encodeCodePoint(inputString, codePoint, output, state))
            {
                return false;
            }
            ++index;
        }
        return true;
    }

    static bool encodeUTF8CodePoint(const String& inputString, uint32_t codePoint, Output& output, CompileState& state)
    {
        if (codePoint <= 0x7f)
        {
            output.push_back(codePoint);
        }
        else
        {
            int bytes = 1;
            int restSize = 6;
            while (codePoint >= (1 << restSize))
            {
                output.push_back(0x80 | (codePoint & 0x3f));
                codePoint >>= 6;
                --restSize;
                ++bytes;
            }
            int mask = ~((1 << (restSize + 1)) - 1);
            output.push_back(mask | codePoint);
            std::reverse(output.end() - bytes, output.end());
        }
        return true;
    }

    static bool encodeUTF16CodePoint(const String& inputString, uint32_t codePoint, Output& output, CompileState& state)
    {
        if (codePoint <= 0xffff)
        {
            output.push_back((uint16_t)codePoint);
        }
        else
        {
            uint16_t upper = (uint32_t)(0xd800 | (codePoint >> 10));
            uint16_t lower = (uint32_t)(0xdc00 | (codePoint & 0x3ff));
            output.push_back(upper);
            output.push_back(lower);
        }
        return true;
    }

    static bool encodeUTF32CodePoint(const String& inputString, uint32_t codePoint, Output& output, CompileState& state)
    {
        output.push_back(codePoint);
        return true;
    }

    static bool encodeCodePoint(const String& inputString, uint32_t codePoint, Output& output, CompileState& state)
    {
        if (codePoint > 0x10ffff || (codePoint >= 0xd800 && codePoint <= 0xdfff))
        {
            //ERROR
            return false;
        }
        if (sizeof(OutType) >= 4)
        {
            return encodeUTF32CodePoint(inputString, codePoint, output, state);
        }
        else if (sizeof(OutType) >= 2)
        {
            return encodeUTF16CodePoint(inputString, codePoint, output, state);
        }
        else if (sizeof(OutType) == 1)
        {
            return encodeUTF8CodePoint(inputString, codePoint, output, state);
        }
        else
        {
            return false;
        }
    }

    static bool stringParseCharacter(const String& inputString, size_t& index, char separator, Output& output, CompileState& state)
    {
        auto chr = inputString[index];
        if (chr == separator)
        {
            invokeError("Found an unescaped separator in a string", index, state);
            return false;
        }
        else
        {
            if (!stringConvertCharacter(inputString, index, output, state))
            {
                return false;
            }
        }
        return true;
    }
};
/**
 * \endinternal
 */

template<typename CharType, typename OutType>
static bool parseString(const std::basic_string<CharType>& inputString, std::vector<OutType>& output, CompileState& state)
{
    if (!StringParserInternal<CharType, OutType>::checkIfWrapped(inputString, '\"', state))
    {
        return false;
    }

    size_t index = 1;
    while (inputString.size() - 1 > index)
    {
        if (!StringParserInternal<CharType, OutType>::stringParseCharacter(inputString, index, '\"', output, state))
        {
            return false;
        }
    }

    return index == inputString.size() - 1;
}

template<typename CharType, typename OutType>
static bool parseCharacter(const std::basic_string<CharType>& inputString, std::vector<OutType>& output, CompileState& state)
{
    if (!StringParserInternal<CharType, OutType>::checkIfWrapped(inputString, '\'', state))
    {
        return false;
    }

    size_t index = 1;
    if (!StringParserInternal<CharType, OutType>::stringParseCharacter(inputString, index, '\'', output, state))
    {
        return false;
    }

    return index == inputString.size() - 1;
}

#endif /* ERAGPSIM_PARSER_STRING_PARSER_HPP_ */
