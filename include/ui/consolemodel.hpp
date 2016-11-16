#ifndef CONSOLEMODEL_H
#define CONSOLEMODEL_H

#include <QObject>
#include <QQmlContext>
#include <QString>
#include <iostream>

#include "core/memory-access.hpp"
#include "common/string-conversions.hpp"



class ConsoleModel: QObject{
    Q_OBJECT

public:
    ConsoleModel(QQmlContext* context, MemoryAccess memoryAccess);
    Q_INVOKABLE QString getText();
    void onDataChanged();
    Q_INVOKABLE void getData();
    Q_INVOKABLE void modeChanged(int newMode);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void changeStart(int newStart);



private:
    QQmlContext* context;
    std::string text;
    int start;
    int maximumLength;
    int mode;
    MemoryAccess _memoryAccess;


signals:
    void dataChanged();
    void textChanged();


};

#endif // CONSOLEMODEL_H
