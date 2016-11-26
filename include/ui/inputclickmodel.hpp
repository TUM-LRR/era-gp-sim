#ifndef INPUTCLICKMODEL_H
#define INPUTCLICKMODEL_H

#include <QObject>
#include <QQmlContext>

#include "core/memory-access.hpp"

class InputClickModel: QObject{
    Q_OBJECT
public:
    InputClickModel(QQmlContext* context, MemoryAccess m);
    Q_INVOKABLE void newClick(int x, int y);
    Q_INVOKABLE QString getStart();
    Q_INVOKABLE void setStart(int start);


private:
    QQmlContext* context;
    int xMouseClick;
    int yMouseClick;
    int start;
    MemoryAccess memoryAccess;

};

#endif // INPUTCLICKMODEL_H
