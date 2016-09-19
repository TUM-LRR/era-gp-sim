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
#include <cassert>
#include <iostream>

#include "arch/common/instruction-information.hpp"
#include "arch/common/instruction-set.hpp"
#include "arch/common/register-container.hpp"
#include "arch/common/register-information.hpp"

EditorComponent::EditorComponent(QQmlContext* projectContext, QObject* parent)
: QObject(parent) {
  projectContext->setContextProperty("editor", this);
  // InstructionSet instructionSet = architectureAccess.getInstructionSet();
  InstructionSet instructionSet;// Placeholder until the line above works

  // TODO select colors according to a theme/possibility to change colors

  //Add all instruction keywords to the syntax highlighter
  QTextCharFormat instructionFormat;
  instructionFormat.setForeground(Qt::darkBlue);
  instructionFormat.setFontWeight(QFont::Bold);
  for (const std::pair<std::string, InstructionInformation>& instructionPair : instructionSet) {
    if (instructionPair.second.hasMnemonic()) {
      // is this check needed?
      std::string keywordRegex;
      // std::string keywordRegex =
      // parserInterface.getSyntaxInstruction(instructionPair.second.getMnemonic());
      //TODO QRegularExpression regex(keywordRegex, QRegularExpression::CaseInsensitiveOption);
      QRegularExpression regex;
      KeywordRule keyword{regex, instructionFormat};
      _keywords.push_back(keyword);
    }
  }

  //Add the immediate regex to the syntax highlighter
  QTextCharFormat immediateFormat;
  immediateFormat.setForeground(Qt::red);
  immediateFormat.setFontWeight(QFont::Bold);

  //std::string immediateRegex = parserInterface.getSyntaxImmediate();
  //_keywords.push_back(KeywordRule{QRegularExpression(immediateRegex), immediateFormat});

  //Add the comment regex to the syntax highlighter
  QTextCharFormat commentFormat;
  commentFormat.setForeground(Qt::green);

  //std::string commentRegex = parserInterface.getSyntaxComment();
  //_keywords.push_back(KeywordRule{QRegularExpression(commentRegex), commentFormat});

  //Add the register regex to the syntax highlighter
  QTextCharFormat registerFormat;
  registerFormat.setForeground(Qt::yellow);
  registerFormat.setFontWeight(QFont::Bold);

  //RegisterContainer registerContainer = architectureAccess.getRegisterSet();
  RegisterContainer registerContainer;

  for(const RegisterInformation& registerInfo : registerContainer) {
      //std::string registerRegex = parserInterface.getSyntaxRegister(registerInfo.getName());
      //_keywords.push_back(KeywordRule{QRegularExpression(registerRegex), registerFormat});
  }

  //Add the label regex to the syntax highlighter
  QTextCharFormat labelFormat;
  labelFormat.setForeground(Qt::red);
  labelFormat.setFontWeight(QFont::Bold);

  //std::string labelRegex = parserInterface.getSyntaxLabel();
  //_keywords.push_back(KeywordRule{QRegularExpression(labelRegex), labelFormat});
}

void EditorComponent::init(QQuickTextDocument* qDocument) {
  if (this->_highlighter) {
    assert(false);
  }
  // test/demonstration of errors
  std::vector<CompileError> list;
  list.push_back(CompileError(
      "<b>error</b>", CodePosition(10, 10), CompileErrorSeverity::ERROR));
  list.push_back(CompileError(
      "warning", CodePosition(20, 20), CompileErrorSeverity::WARNING));
  list.push_back(CompileError(
      "information", CodePosition(30, 30), CompileErrorSeverity::INFORMATION));
  setErrorList(std::move(list));

  // set tab width to 4 spaces
  QTextOption textOptions = qDocument->textDocument()->defaultTextOption();
  QFontMetrics fontMetrics(qDocument->textDocument()->defaultFont());
  textOptions.setTabStop(4 * fontMetrics.width(' '));
  qDocument->textDocument()->setDefaultTextOption(textOptions);

  _highlighter = (std::make_unique<SyntaxHighlighter>(
      std::move(_keywords), qDocument->textDocument()));
  std::cout << "Created new SyntaxHighlighter." << std::endl;
}

void EditorComponent::sendText(QString text) {
}

void EditorComponent::setErrorList(std::vector<CompileError>&& errorList) {
  emit deleteErrors();
  for (const CompileError& error : errorList) {
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
    emit addError(
        QString::fromStdString(error.message()), error.position().first, color);
  }
}

void EditorComponent::setCurrentLine(int line) {
}
