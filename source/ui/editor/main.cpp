#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>

#include "qproject.hpp"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QQmlApplicationEngine engine;

	QProject project;
	// TODO: set only for one component
	engine.rootContext()->setContextProperty("project", &project);
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
