#include "syntaxhighlighter.hpp"
#include <iostream>

SyntaxHighlighter::SyntaxHighlighter(std::vector<KeywordRule> &&keywords,
																		 QTextDocument *document)
: QSyntaxHighlighter(document), _keywords(keywords) {
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
