#ifndef ERAGPSIM_UI_MEMORY_REPRESENTATIONS_H
#define ERAGPSIM_UI_MEMORY_REPRESENTATIONS_H

#include <QObject>
#include <QtQml>
#include <QMetaType>

class MemoryRepresentations : public QObject
{
    Q_OBJECT


    public:
    /**
     * the number format displayed in memory
     */
    enum Representation
    {
       BIN,
       OCT,
       HEX,
       SDEC,
       DEC
    };
    Q_ENUMS(Representation)

        static void declareQML() {
            //qmlRegisterType<MemoryRepresentations>("MemoryRepresentation", 1, 0, "Representation");
            //qmlRegisterUncreatableType<MemoryRepresentations>("MemoryRepresentations", 1, 0,
            //                                             "Representation", "This exports SomeState enums to QML");

        }
};
Q_DECLARE_METATYPE(MemoryRepresentations::Representation)

#endif // ERAGPSIM_UI_MEMORY_REPRESENTATIONS_H
