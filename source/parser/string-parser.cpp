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

#include "parser/string-parser.hpp"
#include <functional>
#include <algorithm>

static bool require(const std::string& inputString, size_t& index)
{
    ++index;
    return index < inputString.size() - 1;
}

static bool isHex(char chr)
{
    return (chr >= '0' && chr <= '9') || (chr >= 'A' && chr <= 'F') || (chr >= 'a' && chr <= 'f');
}

static bool isOctal(char chr)
{
    return chr >= '0' && chr <= '7';
}

static int crawlIndex(const std::string& inputString, size_t& index, std::function<bool(char)> rangeCheck, int maxLength)
{
    int length = 0;
    for (int i = 0; i < maxLength || maxLength == -1; ++i)
    {
        if (!(require(inputString, index) && rangeCheck(inputString[index])))
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

static void encodeCodePoint(uint32_t codePoint, std::vector<char>& output)
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
}

static bool parseControlSequence(const std::string& inputString, size_t& index, std::vector<char>& output)
{
    if (!require(inputString, index))
    {
        return false;
    }
    size_t startIndex = index;
    auto chr = inputString[index];
    switch (chr)
    {
    case 'a':
        output.push_back('\a');
        ++index;
        break;
    case 'b':
        output.push_back('\b');
        ++index;
        break;
    case 'f':
        output.push_back('\f');
        ++index;
        break;
    case 'n':
        output.push_back('\n');
        ++index;
        break;
    case 'r':
        output.push_back('\r');
        ++index;
        break;
    case 't':
        output.push_back('\t');
        ++index;
        break;
    case 'v':
        output.push_back('\v');
        ++index;
        break;
    case '\\':
        output.push_back('\\');
        ++index;
        break;
    case '\'':
        output.push_back('\'');
        ++index;
        break;
    case '\"':
        output.push_back('\"');
        ++index;
        break;
    case '\?':
        output.push_back('\?');
        ++index;
        break;
    case 'u':
        {
            auto len = crawlIndex(inputString, index, isHex, 4);
            if (len != 4)
            {
                return false;
            }
            uint32_t ret = std::stol(inputString.substr(startIndex + 1, len), nullptr, 16);
            encodeCodePoint(ret, output);
        }
        break;
    case 'U':
        {
            auto len = crawlIndex(inputString, index, isHex, 8);
            if (len != 8)
            {
                return false;
            }
            uint32_t ret = std::stol(inputString.substr(startIndex + 1, len), nullptr, 16);
            encodeCodePoint(ret, output);
        }
        break;
    case 'x':
        {
            auto len = std::min(crawlIndex(inputString, index, isHex, -1), 2);
            int ret = std::stoi(inputString.substr(startIndex + 1, len), nullptr, 16);
            output.push_back(ret);
        }
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
        {
            auto len = crawlIndex(inputString, index, isOctal, 2) + 1;
            int ret = std::stoi(inputString.substr(startIndex, len), nullptr, 8);
            if (ret > 0xff)
            {
                return false;
            }
            output.push_back(ret);
        }
        break;
    default:
        return false;
    }
    return true;
}

bool parseString(const std::string& inputString, std::vector<char>& output)
{
    if (inputString.size() < 2)
    {
        return false;
    }
    if (!(inputString[0] == '\"' && inputString[inputString.size() - 1] == '\"'))
    {
        return false;
    }
    size_t index = 1;
    while (index < inputString.size() - 1)
    {
        auto chr = inputString[index];
        if (chr == '\"')
        {
            return false;
        }
        else if (chr == '\\')
        {
            //This is, where the fun begins!
            if (!parseControlSequence(inputString, index, output))
            {
                return false;
            }
        }
        else
        {
            output.push_back(chr);
            ++index;
        }
    }
    return true;
}
