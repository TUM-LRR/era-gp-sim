#ifndef INPUTTEXTMODEL_H
#define INPUTTEXTMODEL_H

#include <QObject>
#include <QQmlContext>

#include "core/memory-access.hpp"

class InputTextModel: QObject
{
    Q_OBJECT
public:
    InputTextModel(QQmlContext* context, MemoryAccess m);
    Q_INVOKABLE void newText(QString text);
    Q_INVOKABLE void setStart(int start);
    Q_INVOKABLE void setMaxLaenge(int maxLaenge);
    Q_INVOKABLE int getMaxLaenge();
    Q_INVOKABLE QString getStart();

private:
    QQmlContext* context;
    int start;
    int maxLaenge;
    MemoryAccess memoryAccess;
};

#endif // INPUTTEXTMODEL_H
