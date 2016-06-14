// This is a dummy class, which manages the syntax highlighter objects for
// different editor instances.
// ONLY FOR TESTING!

#ifndef INCLUDE_UI_EDITOR_QPROJECT_H
#define INCLUDE_UI_EDITOR_QPROJECT_H

#include <QObject>
#include <QQuickTextDocument>
#include <QRegExp>
#include <QTextCharFormat>
#include <QTextDocument>
#include <memory>
#include "syntaxhighlighter.hpp"

class QProject : public QObject {
	Q_OBJECT
 public:
	explicit QProject(QObject *parent = 0);
	Q_INVOKABLE void createHighlighter(QQuickTextDocument *qDocument);

 private:
	std::vector<std::unique_ptr<SyntaxHighlighter>> _highlighters;
	std::vector<KeywordRule> _keywords;
 signals:

 public slots:
};

#endif// INCLUDE_UI_EDITOR_QPROJECT_H
