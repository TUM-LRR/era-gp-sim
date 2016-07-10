#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{//Dummy for the reviewers, will not be added to master-branche
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
