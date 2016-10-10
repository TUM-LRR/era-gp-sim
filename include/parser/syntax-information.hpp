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

#ifndef ERAGPSIM_PARSER_SYNTAX_INFORMATION_HPP
#define ERAGPSIM_PARSER_SYNTAX_INFORMATION_HPP

#include <string>
#include <vector>

/**
 * Provides syntax highlighting information.
 */
class SyntaxInformation {
 public:
  /**
   * Different types of tokens to be highlighted.
   */
  enum class Token { Register, Instruction, Comment, Label, Immediate };

  using TokenRegex = std::pair<const std::string, const Token>;

  /**
   * Helper class which iterates through all regexes for a specified token.
   */
  class TokenIterator
      : public std::iterator<std::input_iterator_tag, std::string> {
    friend class SyntaxInformation;

   public:
    TokenIterator &operator++();
    TokenIterator operator++(int);

    friend bool operator==(const TokenIterator &lhs, const TokenIterator &rhs);
    friend bool operator!=(const TokenIterator &lhs, const TokenIterator &rhs);

    std::string operator*() const;
    const std::string *operator->() const;

   private:
    using VectorIterator = std::vector<TokenRegex>::const_iterator;

    TokenIterator(VectorIterator baseIterator,
                  VectorIterator baseEnd,
                  Token token);

    /**
     * Finds the next regex for #_token. Doesn't change the iterator if it
     * already points to a matching regex.
     */
    void findNext();

    /**
     * Saves the token of this iterator.
     */
    Token _token;

    /**
     * The current and the end iterators of the underlying vector.
     */
    VectorIterator _base_iterator, _base_end;
  };

  /**
   * Helper class which can be used in a for iterator loop.
   */
  class TokenIterable {
    friend class SyntaxInformation;

   public:
    TokenIterator begin() const;
    TokenIterator end() const;

   private:
    using VectorIterator = std::vector<TokenRegex>::const_iterator;
    TokenIterable(VectorIterator begin, VectorIterator end, const Token token)
    : _begin(begin), _end(end), _token(token) {
    }

    VectorIterator _begin, _end;
    const Token _token;
  };

  /**
   * Adds a regex for a specified token.
   *
   * \param regex The regex used for syntax highlighting.
   * \param token The token highlighted by this regex.
   */
  void addSyntaxRegex(const std::string &regex, Token token);

  /**
   * Returns an iterable object for a specified token to be used in a for
   * iterator loop.
   *
   * Example Code:
   * ```
   * //Outputs all Regexes used to highlight instructions.
   * const SyntaxInformation info{...};
   *
   * for(std::string regex :
   *     info.getSyntaxRegex(SyntaxInformation::Token::Instruction)) {
   *   std::cout << regex << '\n';
   * }
   * ```
   *
   * \param token The token to search for.
   * \return Iterable object which can be used in a for loop.
   */
  const TokenIterable getSyntaxRegex(Token token) const;

  /**
   * Returns a reference to the internal list of regexes. Use this if the order
   * of tokens doesn't matter.
   *
   * \return std::vector containing all regexes.
   */
  const std::vector<TokenRegex> &getEntries() const;


 private:
  /**
   * Saves all regexes and their token.
   */
  std::vector<TokenRegex> _entries;
};

#endif /* ERAGPSIM_PARSER_SYNTAX_INFORMATION_HPP */
