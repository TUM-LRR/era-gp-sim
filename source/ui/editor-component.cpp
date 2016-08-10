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

#include <cassert>
#include <QFontMetrics>
#include <iostream>

EditorComponent::EditorComponent(QObject *parent) : QObject(parent) {
    QTextCharFormat format;
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    _keywords.push_back(
            KeywordRule{QRegularExpression("\\badd\\b",
                                                                         QRegularExpression::CaseInsensitiveOption),
                                    format});
    _keywords.push_back(
            KeywordRule{QRegularExpression("\\baddi\\b",
                                                                         QRegularExpression::CaseInsensitiveOption),
                                    format});
    _keywords.push_back(
            KeywordRule{QRegularExpression("\\bmul\\b",
                                                                         QRegularExpression::CaseInsensitiveOption),
                                    format});
    _keywords.push_back(
            KeywordRule{QRegularExpression("\\bdiv\\b",
                                                                         QRegularExpression::CaseInsensitiveOption),
                                    format});
    _keywords.push_back(
            KeywordRule{QRegularExpression("\\band\\b",
                                                                         QRegularExpression::CaseInsensitiveOption),
                                    format});
    _keywords.push_back(KeywordRule{
            QRegularExpression("\\bor\\b", QRegularExpression::CaseInsensitiveOption),
            format});
}

void EditorComponent::init(QQuickTextDocument *qDocument) {
    if(this->_highlighter) {
        assert(false);
    }
    //test
    std::vector<CompileError> list;
    list.push_back(CompileError("error", CodePosition(10, 10), CompileErrorSeverity::ERROR));
    list.push_back(CompileError("warning", CodePosition(20, 20), CompileErrorSeverity::WARNING));
    list.push_back(CompileError("information", CodePosition(30, 30), CompileErrorSeverity::INFORMATION));
    setErrorList(std::move(list));

    // set tab width to 4 spaces
    QTextOption textOptions = qDocument->textDocument()->defaultTextOption();
    QFontMetrics fontMetrics(qDocument->textDocument()->defaultFont());
    textOptions.setTabStop(4 * fontMetrics.width(' '));
    qDocument->textDocument()->setDefaultTextOption(textOptions);

    _highlighter = (std::make_unique<SyntaxHighlighter>(std::move(_keywords), qDocument->textDocument()));
    std::cout << "Created new SyntaxHighlighter." << std::endl;
}

void EditorComponent::sendText(QString text) {

}

void EditorComponent::setErrorList(std::vector<CompileError>&& errorList) {
    emit deleteErrors();
    for(const CompileError& error : errorList) {
        QColor color;
        if(error.severity() == CompileErrorSeverity::ERROR) {
            color = QColor(Qt::red);
        }
        if(error.severity() == CompileErrorSeverity::WARNING) {
            color = QColor(Qt::yellow);
        }
        if(error.severity() == CompileErrorSeverity::INFORMATION) {
            color = QColor(Qt::blue);
        }
        emit addError(QString::fromStdString(error.message()), error.position().first, color);
    }
}

void EditorComponent::setCurrentLine(int line) {

}
