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

#include "ui/qproject.hpp"
#include <QFontMetrics>
#include <QRegularExpression>
#include <QTextOption>
#include <iostream>
#include <memory>

QProject::QProject(QObject *parent) : QObject(parent) {
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

void QProject::createHighlighter(QQuickTextDocument *qDocument) {
	// set tab width to 4 spaces
	QTextOption textOptions = qDocument->textDocument()->defaultTextOption();
	QFontMetrics fontMetrics(qDocument->textDocument()->defaultFont());
	textOptions.setTabStop(4 * fontMetrics.width(' '));
	qDocument->textDocument()->setDefaultTextOption(textOptions);

	_highlighters.push_back(std::unique_ptr<SyntaxHighlighter>(
			new SyntaxHighlighter(std::move(_keywords), qDocument->textDocument())));
	std::cout << "Created new SyntaxHighlighter." << std::endl;
}
