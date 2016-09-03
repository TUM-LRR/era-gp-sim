#ifndef GUIPROJECT_H
#define GUIPROJECT_H

#include <QObject>
#include <string>
#include <functional>
#include <QQmlContext>


class GUIProject: QObject{
    Q_OBJECT
public:
    GUIProject(QQmlContext* context);
    void changeSstem(std::string base);
    std::function<std::string(MemoryValue)> getHexConversion;
    std::function<std::string(MemoryValue)> getBinConversion;
    std::function<std::string(MemoryValue)> getOctConversion;
    std::function<std::string(int)> getSignedDecimalConversion;
    std::function<std::string(int)> getUnsignedDecimalConversion;
    std::function<std::string(float)> getDecimalFloatConversion;
    std::function<int(std::string)> getSignedToMemoryValue;

private:
    //MenubarModel menubarmodel;
    //ToolbarModel toolbarmodel;
    //RegisterModel registermodel;
    //Syntaxhighlighter synhigh;
    //SnapshotModel snapmodel;
    //MemoryModel memorymodel;
    QQmlContext* context;






};

#endif // GUIPROJECT_H
