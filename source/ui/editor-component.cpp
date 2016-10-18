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

#include "ui/editor-component.hpp"

#include <QFontMetrics>

#include "common/assert.hpp"

EditorComponent::EditorComponent(
    QQmlContext *projectContext,
    /* ParserInterface parserInterface, CommandInterface commandInterface,*/ QObject
        *parent)
: QObject(parent) {
  projectContext->setContextProperty("editor", this);

  // TODO select colors according to a theme/possibility to change colors

  // Add all instruction keywords to the syntax highlighter
  QTextCharFormat instructionFormat;
  instructionFormat.setForeground(Qt::darkBlue);
  instructionFormat.setFontWeight(QFont::Bold);
  //_addKeywords(SyntaxInformation::Token::Instruction, instructionFormat,
  //             QRegularExpression::CaseInsensitiveOption, parserInterface);

  // Add the immediate regex to the syntax highlighter
  QTextCharFormat immediateFormat;
  immediateFormat.setForeground(Qt::red);
  immediateFormat.setFontWeight(QFont::Bold);
  //_addKeywords(SyntaxInformation::Token::Immediate, immediateFormat,
  //             QRegularExpression::CaseInsensitiveOption, parserInterface);

  // Add the comment regex to the syntax highlighter
  QTextCharFormat commentFormat;
  commentFormat.setForeground(Qt::green);
  //_addKeywords(SyntaxInformation::Token::Comment, commentFormat,
  //             QRegularExpression::NoPatternOption, parserInterface);

  // Add the register regex to the syntax highlighter
  QTextCharFormat registerFormat;
  registerFormat.setForeground(Qt::yellow);
  registerFormat.setFontWeight(QFont::Bold);
  //_addKeywords(SyntaxInformation::Token::Register, registerFormat,
  //             QRegularExpression::NoPatternOption, parserInterface);

  // Add the label regex to the syntax highlighter
  QTextCharFormat labelFormat;
  labelFormat.setForeground(Qt::red);
  labelFormat.setFontWeight(QFont::Bold);
  //_addKeywords(SyntaxInformation::Token::Label, labelFormat,
  //             QRegularExpression::CaseInsensitiveOption, parserInterface);
}

void EditorComponent::init(QQuickTextDocument *qDocument) {
  if (this->_highlighter) {
    assert::that(false);
  }
  // set tab width to 4 spaces
  QTextOption textOptions = qDocument->textDocument()->defaultTextOption();
  QFontMetrics fontMetrics(qDocument->textDocument()->defaultFont());
  textOptions.setTabStop(4 * fontMetrics.width(' '));
  qDocument->textDocument()->setDefaultTextOption(textOptions);

  _highlighter = (std::make_unique<SyntaxHighlighter>(
      std::move(_keywords), qDocument->textDocument()));
}

void EditorComponent::sendText(QString text) {
}

void EditorComponent::parse() {
  emit parseText();
}

void EditorComponent::setErrorList(std::vector<CompileError> &&errorList) {
  emit deleteErrors();
  for (const CompileError &error : errorList) {
    QColor color;
    if (error.severity() == CompileErrorSeverity::ERROR) {
      color = QColor(Qt::red);
    }
    if (error.severity() == CompileErrorSeverity::WARNING) {
      color = QColor(Qt::yellow);
    }
    if (error.severity() == CompileErrorSeverity::INFORMATION) {
      color = QColor(Qt::blue);
    }
    emit addError(QString::fromStdString(error.message()),
                  error.position().first.line(),
                  color);
  }
}

void EditorComponent::setCurrentLine(int line) {
}

void _addKeywords(SyntaxInformation::Token token,
                  QTextCharFormat format,
                  QRegularExpression::PatternOption patternOption /*,
                  ParserInterface parserInterface*/) {
  /*for (auto &&regexString : parserInterface.getSyntaxRegex(token)) {
    QRegularExpression regex(QString::fromStdString(regexString),
                             patternOption);
    KeywordRule keyword{regex, format};
    _keywords.push_back(keyword);
  }*/
}
