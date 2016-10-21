#ifndef INPUTBUTTONMODEL_H
#define INPUTBUTTONMODEL_H

#include <QObject>
#include <QQmlContext>

class InputButtonModel : public QObject
{
    Q_OBJECT
public:
    InputButtonModel(QQmlContext* context);
    Q_INVOKABLE void buttonClicked(int id);

private:
    QQmlContext* context;
    int start;


};

#endif // INPUTBUTTONMODEL_H
