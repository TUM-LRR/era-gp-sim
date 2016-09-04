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


std::function<std::string(int)> GuiProject::getHexConversion(){
    return hexConversion;
}

std::function<std::string(int)> GuiProject::getBinConversion(){
    return binConversion;
}

std::function<std::string(int)> GuiProject::getOctConversion(){
    return octConversion;
}

std::function<std::string(int)> GuiProject::getSignedDecimalConversion(){
    return signedDecimalConversion;
}

std::function<std::string(int)> GuiProject::getUnsignedDecimalConversion(){
    return unsignedDecimalConversion;
}

std::function<std::string(int)> GuiProject::getDecimalFloatConversion(){
    return decimalFloatConversion;
}

std::function<int(std::string)> GuiProject::getSignedToMemoryValue(){
    return signedToMemoryValue;
}

std::function<int(std::string)> GuiProject::getHexToMemoryValue(){
    return hexToMemoryValue;
}

std::function<int(std::string)> GuiProject::getBinToMemoryValue(){
    return binToMemoryValue;
}

std::function<int(std::string)> GuiProject::getOctToMemoryValue(){
    return octToMemoryValue;
}

std::function<int(std::string)> GuiProject::getUnsignedToMemoryValue(){
    return unsignedToMemoryValue;
}

std::function<int(std::string)> GuiProject::getFloatToMemoryValue(){
    return floatToMemoryValue;
}
