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

    /**
     * \brief Sets the new text which should be stored in the memory
     * \param text the new Text
     */
    Q_INVOKABLE void newText(QString text);


    Q_INVOKABLE void setStart(int start);
    Q_INVOKABLE void setMaxLength(int maxL);
    Q_INVOKABLE int getMaxLength();
    Q_INVOKABLE QString getStart();

private:
    QQmlContext* context;
    int start;
    int maxLength;
    MemoryAccess memoryAccess;
};

#endif // INPUTTEXTMODEL_H
