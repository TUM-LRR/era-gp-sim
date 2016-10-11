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

#include "parser/syntax-information.hpp"

void SyntaxInformation::addSyntaxRegex(const std::string &regex, Token token) {
  _entries.push_back(std::make_pair(regex, token));
}

const SyntaxInformation::TokenIterable
SyntaxInformation::getSyntaxRegex(Token token) const {
  return TokenIterable{_entries.begin(), _entries.end(), token};
}

const std::vector<SyntaxInformation::TokenRegex> &
SyntaxInformation::getEntries() const {
  return _entries;
}


SyntaxInformation::TokenIterator
SyntaxInformation::TokenIterable::begin() const {
  return TokenIterator{_begin, _end, _token};
}

SyntaxInformation::TokenIterator SyntaxInformation::TokenIterable::end() const {
  return TokenIterator{_end, _end, _token};
}


SyntaxInformation::TokenIterator::TokenIterator(VectorIterator baseIterator,
                                                VectorIterator baseEnd,
                                                const Token token)
: _base_iterator(baseIterator), _base_end(baseEnd), _token(token) {
  findNext();
}

SyntaxInformation::TokenIterator &SyntaxInformation::TokenIterator::
operator++() {
  _base_iterator++;
  findNext();
  return *this;
}

SyntaxInformation::TokenIterator SyntaxInformation::TokenIterator::
operator++(int) {
  TokenIterator tmp(*this);
  operator++();
  return tmp;
}

bool operator==(const SyntaxInformation::TokenIterator &lhs,
                const SyntaxInformation::TokenIterator &rhs) {
  return lhs._base_iterator == rhs._base_iterator;
}

bool operator!=(const SyntaxInformation::TokenIterator &lhs,
                const SyntaxInformation::TokenIterator &rhs) {
  return !(lhs == rhs);
}

std::string SyntaxInformation::TokenIterator::operator*() const {
  return _base_iterator->first;
}

const std::string *SyntaxInformation::TokenIterator::operator->() const {
  return &(_base_iterator->first);
}

void SyntaxInformation::TokenIterator::findNext() {
  while (_base_iterator != _base_end && _base_iterator->second != _token)
    ++_base_iterator;
}
