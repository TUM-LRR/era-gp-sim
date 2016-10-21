#ifndef INPUTTEXTMODEL_H
#define INPUTTEXTMODEL_H

#include <QObject>
#include <QQmlContext>

class InputTextModel: QObject
{
    Q_OBJECT
public:
    InputTextModel(QQmlContext* context);
    Q_INVOKABLE void newText(QString text);

private:
    QQmlContext* context;
    int start;
};

#endif // INPUTTEXTMODEL_H
