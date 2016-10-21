#ifndef CONSOLEMODEL_H
#define CONSOLEMODEL_H

#include <QObject>
#include <QQmlContext>
#include <QString>
#include <iostream>

class ConsoleModel: QObject{
    Q_OBJECT

public:
    ConsoleModel(QQmlContext* context);
    Q_INVOKABLE QString getText();
    void onDataChanged();
    Q_INVOKABLE void getData();



private:
    QQmlContext* context;
    std::string text;
    int start;
    int length;

signals:
    void dataChanged();
    void textChanged();


};

#endif // CONSOLEMODEL_H
