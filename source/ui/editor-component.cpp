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

#include <QColor>
#include <QFontMetrics>
#include <QQmlContext>
#include <QQuickTextDocument>
#include <QString>
#include <QTextCharFormat>

#include "arch/common/validation-result.hpp"
#include "common/assert.hpp"
#include "parser/common/compile-error.hpp"
#include "ui/translateable-processing.hpp"

EditorComponent::EditorComponent(QQmlContext *projectContext,
                                 ParserInterface parserInterface,
                                 CommandInterface commandInterface,
                                 QObject *parent)
: _commandInterface(commandInterface)
, _parserInterface(parserInterface)
, QObject(parent) {
  projectContext->setContextProperty("editor", this);
  parserInterface.setSetCurrentLineCallback(
      [this](std::size_t line) { setCurrentLine(line); });

  // TODO select colors according to a theme/possibility to change colors
}

void EditorComponent::init(QQuickTextDocument *qDocument) {
  assert::that(!_highlighter);
  _textDocument = qDocument->textDocument();
  // set tab width to 4 spaces
  QTextOption textOptions = _textDocument->defaultTextOption();
  QFontMetrics fontMetrics(_textDocument->defaultFont());
  textOptions.setTabStop(4 * fontMetrics.width(' '));
  _textDocument->setDefaultTextOption(textOptions);

  _highlighter =
      (std::make_unique<SyntaxHighlighter>(_parserInterface, _textDocument));
}

void EditorComponent::addSecondarySyntaxHighlighter(
    QQuickTextDocument *qDocument) {
  _secondaryHighlighters.push_back(std::make_unique<SyntaxHighlighter>(
      _parserInterface, qDocument->textDocument()));
}

void EditorComponent::deleteSecondarySyntaxHighlighters() {
  _secondaryHighlighters.clear();
}

void EditorComponent::parse(bool force) {
  if (_textChanged || force) {
    _commandInterface.parse(getText().toStdString());
    _textChanged = false;
  }
}

void EditorComponent::setTextChanged(bool value) {
  _textChanged = value;
}

void EditorComponent::setBreakpoint(int line) {
  _commandInterface.setBreakpoint(line);
}

void EditorComponent::deleteBreakpoint(int line) {
  _commandInterface.deleteBreakpoint(line);
}

void EditorComponent::setErrorList(const std::vector<CompileError> &errorList) {
  emit deleteErrors();
  for (const CompileError &error : errorList) {
    QString issueType;
    QColor color;
    switch (error.severity()) {
      case CompileErrorSeverity::ERROR: issueType = "Error"; break;
      case CompileErrorSeverity::WARNING: issueType = "Warning"; break;
      case CompileErrorSeverity::INFORMATION: issueType = "Information"; break;
      default: assert::that(false);
    }
    emit addIssue(
        translate(error.message()), error.position().startLine(), issueType);
  }
}

void EditorComponent::setMacroList(
    const std::vector<MacroInformation> &macroList) {
  QVariantList updatedMacroList;
  for (const auto &macroInformation : macroList) {
    QVariantMap macroInformationMap;
    macroInformationMap["code"] =
        QString::fromStdString(macroInformation.macroCode());
    macroInformationMap["startLine"] =
        QVariant::fromValue(macroInformation.position().startLine() - 1);
    macroInformationMap["endLine"] =
        QVariant::fromValue(macroInformation.position().endLine() - 1);
    int lineCount =
        static_cast<int>(std::count(macroInformation.macroCode().begin(),
                                    macroInformation.macroCode().end(),
                                    '\n'));
    macroInformationMap["lineCount"] = QVariant::fromValue(lineCount);
    macroInformationMap["collapsed"] = QVariant::fromValue(true);
    updatedMacroList.append(macroInformationMap);
  }
  emit updateMacros(updatedMacroList);
}

void EditorComponent::setCurrentLine(int line) {
  emit executionLineChanged(line);
}

QString EditorComponent::getText() {
  emit prepareTextForRetrieval();
  return _textDocument->toPlainText();
}

void EditorComponent::onFinalRepresentationChanged(
    const FinalRepresentation &finalRepresentation) {
  setErrorList(finalRepresentation.errorList().errors());
  setMacroList(finalRepresentation.macroList());
}
