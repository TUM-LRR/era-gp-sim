#include <QGUIApplication>
#include <QQMLContext>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

#include "registermodel.hpp"
#include "qproject.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QString registerDescription =
        "EAX\tAB01CD23\tHH HH HH HH\n"
        " AX\tAB01\tHH HH\n"
        "  AH\tAB\tHH\n"
        "  AL\t01\tHH\n"
        "EBX\t56AB78CD90\tHH HH HH HH\n"
        " BX\t56AB\tHH HH\n"
        "  BH\t56\tHH\n"
        "  BL\tAB\tHH";
    RegisterModel registerModel(registerDescription);

    QQmlApplicationEngine engine;

    QProject project;
    // TODO: set only for one component
    engine.rootContext()->setContextProperty("project", &project);

    engine.rootContext()->setContextProperty("registerModel", &registerModel);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
