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
#include <vector>

#include "core/command-interface.hpp"
#include "parser/common/syntax-information.hpp"
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
   * \param force Set to true to force reparsing even if the code did not
   * change. Defaults to false.
   */
  Q_INVOKABLE void parse(bool force = false);

  /**
   * Set the _textChanged flag to a value.
   *
   * \param value
   */
  Q_INVOKABLE void setTextChanged(bool value);

  /**
   * Adds a breakpoint.
   *
   * \param line The line of the breakpoint.
   */
  Q_INVOKABLE void setBreakpoint(int line);

  /**
   * Deletes a breakpoint.
   *
   * \param line The line of the breakpoint.
   */
  Q_INVOKABLE void deleteBreakpoint(int line);

  /**
   * Set a new list of errors to display in the editor.
   *
   * \param errorList List of CompileError objects.
   *
   */
  void setErrorList(const std::vector<CompileError> &errorList);

  /**
   * Set a new macro list.
   *
   * \param macroList the new macro list.
   */
  void setMacroList(const std::vector<MacroInformation> &macroList);

  /**
   * Set the current line of execution, in order to correctly display it in the
   * editor.
   *
   *  \param line The line number.
   *
   */
  void setCurrentLine(int line);

  // void setMakroList(std::vector<Makro>&& makroList);

  /**
   * Returns the plain text of this editor.
   *
   * \returns The text of this editor as a QString.
   */
  QString getText();

 public slots:
  /**
   * Is called from the finalRepresentationChanged signal in gui project.
   *
   * \param finalRepresentation The new final representation of the core.
   */
  void
  onFinalRepresentationChanged(const FinalRepresentation &finalRepresentation);

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

  /** A boolean to check wether the text was changed since it was last parsed.
   * The text has changed if this flag is true.
   */
  bool _textChanged;

 signals:
  /** A signal to delete all the errors in the editor. */
  void deleteErrors();

  /** A signal to add an error in the editor. */
  void addError(QString message, int line, QColor color);

  /** Change the highlighted line which indicates the execution point. */
  void executionLineChanged(int line);

  /** Update macros in editor. */
  void updateMacros(const QVariantList &macroList);

  /**
   * Sets the text of this editor (replaces all current text)
   *
   * \param text The new text.
   */
  void setText(QString text);

  /**
   * Signals that the cursor moved to another line
   *
   * \param line The new line of the cursor.
   */
  void cursorLineChanged(std::size_t line);

  /**
   * Forces the editor to send a cursorLineChanged signal, even if there was no
   * change.
   */
  void forceCursorUpdate();
};

#endif /* ERAGPSIM_UI_EDITOR_COMPONENT_HPP_ */
