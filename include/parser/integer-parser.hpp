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

#ifndef ERAGPSIM_PARSER_INT_PARSER_HPP
#define ERAGPSIM_PARSER_INT_PARSER_HPP

#include <string>
#include <cctype>
#include <stdexcept>
#include "common/assert.hpp"

#include <iostream>

/**
 * Provides methods for integer parsing.
 * 
 * \tparam T Number type to parse into
 */
template<typename T>
class IntegerParser {
public:
  static T parse(const std::string &str, size_t start = 0, int base = 10, size_t *count = nullptr) {
    return parseInternal(str, start, base, count);
  }
  
  private:
  static T parseInternal(const std::string &str, size_t start, int base, size_t *count) {
    assert::that((base == 0 || base >= 2) && base <= 36);
    
    auto begin = str.begin() + start;
    auto position = begin;
    bool negative = false;
    T value{};
    
    //Skip all spaces
    while(std::isspace(*position)) {
      ++position;
    }
    
    //Check sign
    if(*position == '-') {
      negative = true;
      ++position;
    } else if(*position == '+') {
      ++position;
    }
    
    //Check prefixes
    if(*position == '0') {
      ++position;
      
      if(*position == 'x' || *position == 'X' ) {
        if(base == 0 || base == 16) {
          base = 16;
          ++position;//Skip the 'x'
        }
      } else if(base == 0) {
         base = 8;
      }
    }
    
    if(base == 0) base = 10;
    
    for(; position != str.end(); ++position) {
      T oldValue = value;
      
      char c = *position;
      int digit = -1;
      if(c >= '0' && c <= '9') {
        digit = c - '0';
      } else if(c >= 'a' && c <= 'z') {
        digit = 10 + c - 'a';
      } else if(c >= 'A' && c <= 'Z') {
        digit = 10 + c - 'A';
      }
      
      //Check if digit is inside range
      if(digit < 0 || digit >= base){
        break;
      }
      
      //Shift value one to the left and add new digit.
      value *= base;
      value += T(digit);
      
      //If the new value is smaller than before, there was an overflow.
      if(value < oldValue) {
          throw std::out_of_range{"Integer out of range"};
      }
    }
    
    //If no characters were processed, throw exception
    if(position == begin) throw std::invalid_argument{"No conversion performed"};
    
    //Return amount of processed characters
    if(count != nullptr) *count = (position - begin);
    
    if(negative) value = -value;
    
    return value;
  }

};

// Specializations using the standard conversion methods
template<>
int IntegerParser<int>::parseInternal(const std::string &str, size_t start, int base, size_t *count);

template<>
long IntegerParser<long>::parseInternal(const std::string &str, size_t start, int base, size_t *count);

template<>
long long IntegerParser<long long>::parseInternal(const std::string &str, size_t start, int base, size_t *count);

template<>
unsigned long IntegerParser<unsigned long>::parseInternal(const std::string &str, size_t start, int base, size_t *count);

template<>
unsigned long long IntegerParser<unsigned long long>::parseInternal(const std::string &str, size_t start, int base, size_t *count);

#endif /* ERAGPSIM_PARSER_INT_PARSER_HPP */
