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

#ifndef ERAGPSIM_UI_EDITOR_COMPONENT_HPP_
#define ERAGPSIM_UI_EDITOR_COMPONENT_HPP_

#include <QObject>
#include <QTextDocument>
#include <memory>

#include "core/command-interface.hpp"
#include "parser/syntax-information.hpp"
#include "ui/syntaxhighlighter.hpp"

class ParserInterface;
class ValidationResult;
class QQmlContext;
class QQuickTextDocument;
class QTextCharFormat;
class QColor;
class CompileError;

/**
 * This class is the c++ component for the QML Editor and manages its
 * communication to other parts of the ui and the core.
 *
 */
class EditorComponent : public QObject {
  Q_OBJECT
 public:
  explicit EditorComponent(QQmlContext *projectContext,
                           ParserInterface parserInterface,
                           CommandInterface commandInterface,
                           QObject *parent = 0);

  /**
   * \brief creates a new syntax-highlighter for this editor
   *
   * should only be called once at initialization of the editor
   *
   * \param qDocument
   */
  Q_INVOKABLE void init(QQuickTextDocument *qDocument);

  /**
   * Invokes the parser with the current text of the editor.
   *
   */
  Q_INVOKABLE void parse();

  /**
   * Set a new list of errors to display in the editor.
   *
   * \param errorList List of CompileError objects.
   *
   */
  void setErrorList(const std::vector<CompileError> &errorList);

  /**
   * Set the current line of execution, in order to correctly display it in the
   * editor.
   *
   *  \param line The line number.
   *
   */
  void setCurrentLine(int line);

  /**
   * Shows a runtime error in the ui.
   *
   * \param validationResult The validation result which indicated the error.
   */
  void throwRuntimeError(const ValidationResult &validationResult);

  // void setMakroList(std::vector<Makro>&& makroList);

 private:
  /**
   * Adds the all keywords for a token to the keyword list.
   *
   * \param token Token to select which type of keywords to add.
   * \format The format for the keywords.
   * \param patternOption Option for the regex, for example
   *QRegularExpression::CaseInsensitiveOption.
   * \param parserInterface ParserInterface to access the SyntaxInformation
   *object.
   */
  void _addKeywords(SyntaxInformation::Token token,
                    QTextCharFormat format,
                    QRegularExpression::PatternOption patternOption,
                    ParserInterface parserInterface);

  /** The syntax Highlighter of this editor. Is initialized in the init()
   * method. */
  std::unique_ptr<SyntaxHighlighter> _highlighter;

  /** A list of keywords to initialize the syntax highlighter. */
  std::vector<KeywordRule> _keywords;

  /** The command interface of the core */
  CommandInterface _commandInterface;

  /** A pointer to the QTextDocument used by the editor. */
  QTextDocument *_textDocument;

 signals:
  /** A signal to delete all the errors in the editor. */
  void deleteErrors();

  /** A signal to add an error in the editor. */
  void addError(QString message, int line, QColor color);

  /** Change the highlighted line which indicates the execution point. */
  void executionLineChanged(int line);

  /** display a runtime error in the ui. */
  void runtimeError(QString errorMessage);
};

#endif /* ERAGPSIM_UI_EDITOR_COMPONENT_HPP_ */
