#include "qproject.hpp"
#include <QRegularExpression>
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
	_highlighters.push_back(std::unique_ptr<SyntaxHighlighter>(
			new SyntaxHighlighter(std::move(_keywords), qDocument->textDocument())));
	std::cout << "Created new SyntaxHighlighter." << std::endl;
}
