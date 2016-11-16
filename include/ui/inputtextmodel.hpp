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

private:
    QQmlContext* context;
    int start;
    MemoryAccess memoryAccess;
};

#endif // INPUTTEXTMODEL_H
