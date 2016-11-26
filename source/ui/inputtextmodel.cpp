#include <iostream>

#include "ui/inputtextmodel.hpp"
#include "core/conversions.hpp"

InputTextModel::InputTextModel(QQmlContext* context, MemoryAccess m): QObject(), context(context), memoryAccess(m)
{
    context->setContextProperty("inputtextMod", this);
}

void InputTextModel::newText(QString text){
    std::string stdText=text.toStdString();
    std::cout<<stdText<<std::endl;


    for(unsigned i=0; i<stdText.length(); i++){
        char z=stdText.at(i);
        unsigned zahl=int(z);
        if(zahl<225){//else do nothing, was not a ascii sign
            //std::cout<<zahl<<std::endl;
            MemoryValue m=conversions::convert(i);
            memoryAccess.setMemoryValueAt(start+i, m, 8);
        }
    }
}

void InputTextModel::setStart(int start){
    if(memoryAccess.getMemorySize().get() >= start+maxLaenge){
        this->start=start;
    }
}

QString InputTextModel::getStart(){
    return QString::fromStdString(std::to_string(start));
}

void InputTextModel::setMaxLaenge(int maxLaenge){
    this->maxLaenge=maxLaenge;
}

int InputTextModel::getMaxLaenge(){
    return maxLaenge
}
