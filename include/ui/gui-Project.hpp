#ifndef GUIPROJECT_H
#define GUIPROJECT_H

#include <QObject>
#include <string>
#include <functional>
#include <QQmlContext>


class GuiProject: QObject{
    Q_OBJECT
public:
    GuiProject(QQmlContext* context/*, Core-Project*/);
    void changeSystem(std::string base);
    void run();
    void runLine();
    void runBreakpoint();
    void stop();
    void save();
    void saveAs(QByteArray name);
    void snapshot(QByteArray name);

    std::function<std::string(MemoryValue)> getHexConversion();
    std::function<std::string(MemoryValue)> getBinConversion();
    std::function<std::string(MemoryValuent)> getOctConversion();
    std::function<std::string(MemoryValue)> getSignedDecimalConversion();
    std::function<std::string(MemoryValue)> getUnsignedDecimalConversion();
    std::function<std::string(MemoryValue)> getDecimalFloatConversion();

    std::function<MemoryValue(std::string)> getSignedToMemoryValue();
    std::function<MemoryValue(std::string)> getHexToMemoryValue();
    std::function<MemoryValue(std::string)> getBinToMemoryValue();
    std::function<MemoryValue(std::string)> getOctToMemoryValue();
    std::function<MemoryValue(std::string)> getUnsignedToMemoryValue();
    std::function<MemoryValue(std::string)> getFloatToMemoryValue();


private:
    //RegisterModel registermodel;
    //Syntaxhighlighter synhigh;
    //SnapshotModel snapmodel;
    //MemoryModel memorymodel;
    //Core-Project
    QQmlContext* context;
    std::function<std::string(MemoryValue)> hexConversion;
    std::function<std::string(MemoryValue)> binConversion;
    std::function<std::string(MemoryValue)> octConversion;
    std::function<std::string(MemoryValue)> signedDecimalConversion;
    std::function<std::string(MemoryValue)> unsignedDecimalConversion;
    std::function<std::string(MemoryValue)> decimalFloatConversion;

    std::function<MemoryValue(std::string)> signedToMemoryValue;
    std::function<MemoryValue(std::string)> hexToMemoryValue;
    std::function<MemoryValue(std::string)> binToMemoryValue;
    std::function<MemoryValue(std::string)> octToMemoryValue;
    std::function<MemoryValue(std::string)> unsignedToMemoryValue;
    std::function<MemoryValue(std::string)> floatToMemoryValue;







};

#endif // GUIPROJECT_H
