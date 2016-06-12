#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <iostream>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject *rootObject = engine.rootObjects().first();
    QObject *obj = rootObject->findChild<QObject*>("list");


    //QObject *obj =engine.findChild<QObject *>("mainWindow");
    if(obj!=NULL){
        QVariant data = "Orange";
           bool ret = QMetaObject::invokeMethod(obj, "add",
               Q_ARG(QVariant, data));
           //std::cout<<ret<<std::endl;
           if (!ret){
               std::cout<<"Fehler"<<std::endl;
           }
           QVariant data2=1;
           bool ret2 = QMetaObject::invokeMethod(obj, "remove",
               Q_ARG(QVariant, data2));
           //std::cout<<ret<<std::endl;
           if (!ret2){
               std::cout<<"Fehler"<<std::endl;
           }
    }else{
        std::cout<<"Kein Objekt"<<std::endl;
    }





    //std::cout << "Ende" << std::endl;
    return app.exec();
}
