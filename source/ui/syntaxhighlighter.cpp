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

#include "ui/syntaxhighlighter.hpp"
#include <QQuickTextDocument>
#include <QString>
#include <QTextCharFormat>
#include <iostream>
#include "core/parser-interface.hpp"

SyntaxHighlighter::SyntaxHighlighter(ParserInterface parserInterface,
                                     QTextDocument *document)
: QSyntaxHighlighter(document) {
  // Add all instruction keywords to the syntax highlighter
  QTextCharFormat instructionFormat;
  instructionFormat.setForeground(Qt::darkBlue);
  instructionFormat.setFontWeight(QFont::Bold);
  _addKeywords(SyntaxInformation::Token::Instruction,
               instructionFormat,
               QRegularExpression::CaseInsensitiveOption,
               parserInterface);

  // Add the immediate regex to the syntax highlighter
  QTextCharFormat immediateFormat;
  immediateFormat.setForeground(Qt::red);
  immediateFormat.setFontWeight(QFont::Bold);
  _addKeywords(SyntaxInformation::Token::Immediate,
               immediateFormat,
               QRegularExpression::CaseInsensitiveOption,
               parserInterface);

  // Add the comment regex to the syntax highlighter
  QTextCharFormat commentFormat;
  commentFormat.setForeground(Qt::darkGreen);
  _addKeywords(SyntaxInformation::Token::Comment,
               commentFormat,
               QRegularExpression::NoPatternOption,
               parserInterface);

  // Add the register regex to the syntax highlighter
  QTextCharFormat registerFormat;
  registerFormat.setForeground(QColor::fromRgb(177, 137, 4));
  registerFormat.setFontWeight(QFont::Bold);
  _addKeywords(SyntaxInformation::Token::Register,
               registerFormat,
               QRegularExpression::NoPatternOption,
               parserInterface);

  // Add the label regex to the syntax highlighter
  QTextCharFormat labelFormat;
  labelFormat.setForeground(Qt::red);
  labelFormat.setFontWeight(QFont::Bold);
  _addKeywords(SyntaxInformation::Token::Label,
               labelFormat,
               QRegularExpression::CaseInsensitiveOption,
               parserInterface);
}

void SyntaxHighlighter::highlightBlock(const QString &text) {
  for (uint i = 0; i < _keywords.size(); i++) {
    const KeywordRule &rule = _keywords.at(i);
    QRegularExpressionMatchIterator it = rule.rulePattern.globalMatch(text);
    while (it.hasNext()) {
      QRegularExpressionMatch match = it.next();
      setFormat(
          match.capturedStart(), match.capturedLength(), rule.ruleTextFormat);
    }
  }
}

void SyntaxHighlighter::_addKeywords(
    SyntaxInformation::Token token,
    QTextCharFormat format,
    QRegularExpression::PatternOption patternOption,
    ParserInterface parserInterface) {
  for (const auto &regexString : parserInterface.getSyntaxRegex(token).get()) {
    QRegularExpression regex(QString::fromStdString(regexString),
                             patternOption);
    KeywordRule keyword{regex, format};
    _keywords.push_back(keyword);
  }
}
