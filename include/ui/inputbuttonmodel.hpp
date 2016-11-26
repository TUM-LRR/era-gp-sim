#ifndef INPUTBUTTONMODEL_H
#define INPUTBUTTONMODEL_H

#include <QObject>
#include <QQmlContext>

#include "core/memory-access.hpp"

class InputButtonModel : public QObject
{
    Q_OBJECT
public:
    InputButtonModel(QQmlContext* context, MemoryAccess m);
    Q_INVOKABLE void buttonClicked(int id);
    Q_INVOKABLE QString getStart();
    Q_INVOKABLE void setStart(int start);

private:
    QQmlContext* context;
    int start;
    MemoryAccess memoryAccess;


};

#endif // INPUTBUTTONMODEL_H
