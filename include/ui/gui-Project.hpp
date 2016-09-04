#ifndef GUIPROJECT_H
#define GUIPROJECT_H

#include <QObject>
#include <string>
#include <functional>
#include <QQmlContext>


class GuiProject: QObject{
    Q_OBJECT
public:
    GuiProject(QQmlContext* context);
    void changeSystem(std::string base);

    std::function<std::string(int)> getHexConversion();
    std::function<std::string(int)> getBinConversion();
    std::function<std::string(int)> getOctConversion();
    std::function<std::string(int)> getSignedDecimalConversion();
    std::function<std::string(int)> getUnsignedDecimalConversion();
    std::function<std::string(int)> getDecimalFloatConversion();

    std::function<int(std::string)> getSignedToMemoryValue();
    std::function<int(std::string)> getHexToMemoryValue();
    std::function<int(std::string)> getBinToMemoryValue();
    std::function<int(std::string)> getOctToMemoryValue();
    std::function<int(std::string)> getUnsignedToMemoryValue();
    std::function<int(std::string)> getFloatToMemoryValue();


private:
    //MenubarModel menubarmodel;
    //ToolbarModel toolbarmodel;
    //RegisterModel registermodel;
    //Syntaxhighlighter synhigh;
    //SnapshotModel snapmodel;
    //MemoryModel memorymodel;
    QQmlContext* context;
    std::function<std::string(int)> hexConversion;
    std::function<std::string(int)> binConversion;
    std::function<std::string(int)> octConversion;
    std::function<std::string(int)> signedDecimalConversion;
    std::function<std::string(int)> unsignedDecimalConversion;
    std::function<std::string(int)> decimalFloatConversion;

    std::function<int(std::string)> signedToMemoryValue;
    std::function<int(std::string)> hexToMemoryValue;
    std::function<int(std::string)> binToMemoryValue;
    std::function<int(std::string)> octToMemoryValue;
    std::function<int(std::string)> unsignedToMemoryValue;
    std::function<int(std::string)> floatToMemoryValue;







};

#endif // GUIPROJECT_H
