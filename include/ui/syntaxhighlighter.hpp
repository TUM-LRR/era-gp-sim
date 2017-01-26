/*
* ERASIM Assembler Interpreter
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

#ifndef INCLUDE_UI_EDITOR_SYNTAXHIGHLIGHTER_H
#define INCLUDE_UI_EDITOR_SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <vector>

#include "parser/common/syntax-information.hpp"
#include "ui/keyword-rule.hpp"

class ParserInterface;
class QQuickTextDocument;
class QTextCharFormat;

/**
 * An implementation of a syntax highlighter for qml.
 */
class SyntaxHighlighter : public QSyntaxHighlighter {
  Q_OBJECT
 public:
  /** Creates a new SyntaxHighlighter.
   *
   * \param ruleList List of keywords for the syntax highlighter.
   * \param document A pointer to the QTextDocument for this highlighter.
   */
  SyntaxHighlighter(ParserInterface &parserInterface, QTextDocument *document);

 protected:
  /**
   * Overrides method of QSyntaxHighlighter.
   * This method highlights a block of text.
   *
   * \param text The text which is highlighted by this method.
   */
  void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

 private:
  /**
   * \brief _addKeywords Adds the regex to highlight certain pieces of syntax
   * information (e.g. immediates, instructions etc.) to the list of _keywords
   * along with the associated format information.
   * \param token Type of syntax information (e.g. immediates, instrucitons,
   * registers etc.)
   * \param format The format that is supposed to be used to highlight these
   * patterns.
   * \param patternOption Options for defining how the pattern is supposed to be
   * interpreted (e.g. CaseInsensitive).
   * \param parserInterface Parser Interface for fetching the regex for the
   * given syntax information.
   */
  void _addKeywords(SyntaxInformation::Token token,
                    QTextCharFormat format,
                    QRegularExpression::PatternOption patternOption,
                    ParserInterface &parserInterface);

  /**
   * Adds regex for highlighting immediates to the syntax highlighter.
   * \param parserInterface ParserInterface for retrieving regex.
   */
  void _addImmediateRegexToSyntaxHighlighter(ParserInterface &parserInterface);

  /**
   * Adds regex for highlighting instruction keywords to the syntax highlighter.
   * \param parserInterface ParserInterface for retrieving regex.
   */
  void _addInstructionKeywordsRegexToSyntaxHighlighter(
      ParserInterface &parserInterface);

  /**
   * Adds regex for highlighting comments to the syntax highlighter.
   * \param parserInterface ParserInterface for retrieving regex.
   */
  void _addCommentRegexToSyntaxHighlighter(ParserInterface &parserInterface);

  /**
   * Adds regex for highlighting registers to the syntax highlighter.
   * \param parserInterface ParserInterface for retrieving regex.
   */
  void _addRegisterRegexToSyntaxHighlighter(ParserInterface &parserInterface);

  /**
   * Adds regex for highlighting labels to the syntax highlighter.
   * \param parserInterface ParserInterface for retrieving regex.
   */
  void _addLabelRegexToSyntaxHighlighter(ParserInterface &parserInterface);

  /** A list of all keywords to highlight. */
  std::vector<KeywordRule> _keywords;
};

#endif  // INCLUDE_UI_EDITOR_SYNTAXHIGHLIGHTER_H
