#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <iostream>
#include <QQMLContext>
#include <QQmlEngine>


#include "toolbarmodel.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    ToolbarModel toolbarmodel;

    engine.rootContext()->setContextProperty("toolbarModel",  &toolbarmodel);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
