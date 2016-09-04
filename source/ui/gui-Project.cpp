#include <functional>

#include "gui-Project.hpp"

GuiProject::GuiProject(QQmlContext* context): QObject(), context(context){
    //An alle Komponenten weitergeben
    //alle Komponenten initialisieren
    //Alle Functions initialisieren
}

void GuiProject::changeSystem(std::string base){
    //Alle Komponenten informieren
}


std::function<std::string (MemoryValue)> GuiProject::getHexConversion(){
    return hexConversion;
}

std::function<std::string(MemoryValue)> GuiProject::getBinConversion(){
    return binConversion;
}

std::function<std::string(MemoryValue)> GuiProject::getOctConversion(){
    return octConversion;
}

std::function<std::string(MemoryValue)> GuiProject::getSignedDecimalConversion(){
    return signedDecimalConversion;
}

std::function<std::string(MemoryValue)> GuiProject::getUnsignedDecimalConversion(){
    return unsignedDecimalConversion;
}

std::function<std::string(MemoryValue)> GuiProject::getDecimalFloatConversion(){
    return decimalFloatConversion;
}

std::function<MemoryValue(std::string)> GuiProject::getSignedToMemoryValue(){
    return signedToMemoryValue;
}

std::function<MemoryValue(std::string)> GuiProject::getHexToMemoryValue(){
    return hexToMemoryValue;
}

std::function<MemoryValue(std::string)> GuiProject::getBinToMemoryValue(){
    return binToMemoryValue;
}

std::function<MemoryValue(std::string)> GuiProject::getOctToMemoryValue(){
    return octToMemoryValue;
}

std::function<MemoryValue(std::string)> GuiProject::getUnsignedToMemoryValue(){
    return unsignedToMemoryValue;
}

std::function<MemoryValue(std::string)> GuiProject::getFloatToMemoryValue(){
    return floatToMemoryValue;
}
