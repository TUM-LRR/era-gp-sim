#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <iostream>
#include <QQMLContext>
#include <QQmlEngine>

#include "menubarmodel.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    MenubarModel menubarmodel(engine.rootContext());

    //engine.rootContext()->setContextProperty("menubarModel",  &menubarmodel);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));


    return app.exec();
}
