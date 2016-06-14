#ifndef INCLUDE_UI_EDITOR_SYNTAXHIGHLIGHTER_H
#define INCLUDE_UI_EDITOR_SYNTAXHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <vector>

struct KeywordRule {
	QRegularExpression rulePattern;
	QTextCharFormat ruleTextFormat;
};

class SyntaxHighlighter : QSyntaxHighlighter {
	Q_OBJECT
 public:
	SyntaxHighlighter(std::vector<KeywordRule> &&ruleList,
										QTextDocument *document);
	~SyntaxHighlighter() = default;

 protected:
	void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

 private:
	std::vector<KeywordRule> _keywords;
};

#endif// INCLUDE_UI_EDITOR_SYNTAXHIGHLIGHTER_H
