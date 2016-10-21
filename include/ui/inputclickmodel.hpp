#ifndef INPUTCLICKMODEL_H
#define INPUTCLICKMODEL_H

#include <QObject>
#include <QQmlContext>

class InputClickModel: QObject{
    Q_OBJECT
public:
    InputClickModel(QQmlContext* context);
    Q_INVOKABLE void newClick(int x, int y);


private:
    QQmlContext* context;
    int xMouseClick;
    int yMouseClick;
    int start;


};

#endif // INPUTCLICKMODEL_H
